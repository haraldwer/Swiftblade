#include "SDRInstance.h"

#include "opencv2/video.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/ocl.hpp"

void SDRInstance::Init()
{
    Instance::Init();
    StartThread();
}

void SDRInstance::Deinit()
{
    StopThread();
    Instance::Deinit();
}

void SDRInstance::DrawPanel()
{
    ImGui::Text("Config");
    bool reload = config.Edit();
    
    threadLock.lock();
    
    // Cameras
    ImGui::Separator();
    ImGui::Text("Available cameras: %i", static_cast<int>(frameData.numCameras));
    if (frameData.numCameras < 2)
    {
        ImGui::Text("Connect at least two cameras.");
    }
    else
    {
        ImGui::Text("Camera status:");
        bool leftStatus = frameData.left;
        ImGui::Checkbox("LEFT", &leftStatus);
        bool rightStatus = frameData.right;
        ImGui::Checkbox("RIGHT", &rightStatus);
    }
    
    // Debug
    ImGui::Separator();
    ImGui::Text("NumPoints %i", static_cast<int>(frameData.points));
    
    threadLock.unlock();
    
    if (ImGui::Button("Reload", ImVec2(-1, 0)) || reload)
    {
        StopThread();
        StartThread();
    }
}

void SDRInstance::StartThread()
{
    run = true;
    mainThread = std::thread([&]
    {
        SDRContext context;
        context.config = config;
        if (context.Init())
        {
            while (run)
            {
                context.Frame();
                threadLock.lock();
                frameData = context.frame;
                threadLock.unlock();
            }
        }
        context.Deinit();
    });
}

void SDRInstance::StopThread()
{
    run = false; 
    if (mainThread.joinable())
        mainThread.join();
}

bool SDRContext::Init()
{
    // Check OpenCL availability
    CHECK_RETURN_LOG(!cv::ocl::haveOpenCL(), "OpenCL not available", false);
    cv::ocl::setUseOpenCL(true);
    
    int threadCount = std::thread::hardware_concurrency();
    int numThreads = Utility::Math::Max(1, threadCount * 3 / 4);
    cv::setNumThreads(numThreads);
    LOG("CV using num threads: ", numThreads);
    
    // Query available cameras
    availableCameras.clear();
    for (int i = 0; i < config.CameraQueryCount; ++i) 
    {
        cv::VideoCapture cap(i);
        if (cap.isOpened()) {
            availableCameras.push_back(i);
            cap.release();
        }
    }
    frame.numCameras = availableCameras.size();
    CHECK_RETURN_LOG(availableCameras.size() < 2, "Not enough cameras connected", false);
    
    // Stereo BM (T-API)
    stereo = cv::StereoBM::create(config.StereoNumDisparities, config.StereoBlockSize);
    
    // Create cameras
    capL = cv::VideoCapture(availableCameras.at(0));
    capR = cv::VideoCapture(availableCameras.at(1));
    frame.left = capL.isOpened();
    frame.right = capR.isOpened();
    
    CHECK_RETURN_LOG(!capL.isOpened() || !capL.isOpened(), "Failed to open cameras", false);
    
    // Calibrate
    const int width = capL.get(cv::CAP_PROP_FRAME_WIDTH);
    const int height = capL.get(cv::CAP_PROP_FRAME_HEIGHT);
    const double fovRad = config.CameraFOV * CV_PI / 180.0; // convert to radians
    const double fx = width / (2.0 * tan(fovRad / 2.0));
    const double fy = fx;
    const double cx = width/2.0;
    const double cy = height/2.0;
    K = (cv::Mat_<double>(3,3) << 
        fx, 0, cx,
       0, fy, cy,
       0, 0, 1);
    
    return true;
}

void SDRContext::Deinit()
{
    // Release stuff
}

void SDRContext::Frame() 
{
    CHECK_RETURN(!stereo);
    CHECK_RETURN(!capL.isOpened() || !capR.isOpened());

    // 1. Capture frames
    capL >> frameL;
    capR >> frameR;
    CHECK_RETURN(frameL.empty() || frameR.empty());

    // 2. Convert to grayscale
    cv::cvtColor(frameL, grayL, cv::COLOR_BGR2GRAY);
    cv::cvtColor(frameR, grayR, cv::COLOR_BGR2GRAY);

    // 3. Downscale for tracking / LK
    cv::resize(grayL, graySmallL, cv::Size(), config.Scale, config.Scale);
    cv::resize(grayR, graySmallR, cv::Size(), config.Scale, config.Scale);

    // 4. Allocate / reuse GPU memory
    if (uLeft.empty() || uLeft.size() != graySmallL.size())
        uLeft.create(graySmallL.size(), graySmallL.type());
    if (uRight.empty() || uRight.size() != graySmallR.size())
        uRight.create(graySmallR.size(), graySmallR.type());
    if (uDisp.empty() || uDisp.size() != graySmallL.size())
        uDisp.create(graySmallL.size(), CV_16S);
    if (uDepth.empty() || uDepth.size() != graySmallL.size())
        uDepth.create(graySmallL.size(), CV_32F);

    graySmallL.copyTo(uLeft);
    graySmallR.copyTo(uRight);

    // 5. Compute stereo disparity on GPU
    stereo->compute(uLeft, uRight, uDisp);

    // 6. Depth on GPU: depth = f * B / disparity
    cv::UMat dispFloat;
    uDisp.convertTo(dispFloat, CV_32F, 1.0 / 16.0);           // BM scale
    const float baseline = config.StereoCameraDistance;
    const float focalLength = (K.at<double>(1, 1) + K.at<double>(2, 2)) / 2;
    cv::UMat fB(uDepth.size(), CV_32F, cv::Scalar(focalLength * baseline));
    cv::divide(fB, dispFloat, uDepth);

    // Mask invalid disparities
    cv::UMat mask;
    cv::compare(dispFloat, 0.0f, mask, cv::CMP_LE);
    uDepth.setTo(cv::Scalar(0.0f), mask);

    // 7. Feature detection (downscaled)
    if (featureUpdateTimer.Ellapsed() > 5.0f || pointsPrev.size() < 10)
    {
        cv::goodFeaturesToTrack(graySmallL, pointsPrev, 200, 0.01, 10);
        featureUpdateTimer = {};
    }

    // 8. LK optical flow (downscaled)
    if (prevGraySmall.empty())
        graySmallL.copyTo(prevGraySmall);

    cv::calcOpticalFlowPyrLK(prevGraySmall, graySmallL, pointsPrev, pointsCurr, status, err);

    // 9. Prepare 3D points for PnP (sparse)
    objectPoints.clear();
    imagePoints.clear();
    const float scale = config.Scale;
    for (size_t i = 0; i < pointsCurr.size(); ++i)
    {
        if (!status[i])
            continue;
        cv::Point2f pt = pointsCurr[i];
        float d = uDepth.getMat(cv::ACCESS_READ).at<float>(cv::Point(pt.x, pt.y));
        if (d <= 0.0f) continue;
        cv::Vec3f xyz((pt.x * d) / focalLength, (pt.y * d) / focalLength, d); // camera-space
        objectPoints.push_back(cv::Point3f(xyz));
        imagePoints.push_back(pt / scale); // full-res coordinates
    }
    
    // 10. Solve PnP for camera pose
    cv::Mat rotation;
    cv::Mat translation;
    if (!objectPoints.empty())
    {
        cv::Mat rvec, tvec;
        cv::solvePnP(objectPoints, imagePoints, K, cv::Mat(), rvec, tvec, false, cv::SOLVEPNP_ITERATIVE);
        cv::Rodrigues(rvec, rotation); // rotation
        translation = tvec;               // translation
    }

    // 11. Draw tracking for debug (optional)
    for (size_t i = 0; i < pointsCurr.size(); ++i)
        if (status[i])
        {
            cv::Point2f pt = pointsCurr[i] * (1.0f / scale); // scale up
            cv::circle(frameL, pt, 3, cv::Scalar(0, 255, 0), -1);
        }

    pointsPrev = pointsCurr;
    graySmallL.copyTo(prevGraySmall);
    frame.points = pointsCurr.size();

    // 12. Display full-resolution frame (debug)
    cv::imshow("Stereo + VO (T-API)", frameL);
    cv::waitKey(1);
}
