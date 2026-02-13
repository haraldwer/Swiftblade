#include "SDRInstance.h"

#include "opencv2/video.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/ocl.hpp"

void SDRInstance::Init()
{
    Instance::Init();
    
    LOG("OpenGL support: ", cv::getBuildInformation());
    
    // Check OpenCL availability
    CHECK_RETURN_LOG(!cv::ocl::haveOpenCL(), "OpenCL not available");
    cv::ocl::setUseOpenCL(true);
    
    int threadCount = std::thread::hardware_concurrency();
    int numThreads = Utility::Math::Max(1, threadCount * 3 / 4);
    cv::setNumThreads(numThreads);
    LOG("CV using num threads: ", numThreads);
    
    StartThread();
}

void SDRInstance::Deinit()
{
    StopThread();
    Instance::Deinit();
}

void SDRInstance::DrawPanel()
{
    PROFILE();
    
    if (!cv::ocl::haveOpenCL())
    {
        ImGui::Text("OpenCL not available");
        return;
    }
    
    ImGui::SeparatorText("Config");
    ImGui::Text("Config");
    config.Edit();
    
    threadLock.lock();
    auto data = frameData;
    threadLock.unlock();
    
    ImGui::SeparatorText("Cameras");
    ImGui::Text("Available cameras: %i", static_cast<int>(data.numCameras));
    if (data.numCameras < 2)
    {
        ImGui::Text("Connect at least two cameras.");
    }
    else
    {
        ImGui::Text("Camera status:");
        bool leftStatus = data.left;
        ImGui::Checkbox("LEFT", &leftStatus);
        bool rightStatus = data.right;
        ImGui::Checkbox("RIGHT", &rightStatus);
    }
    
    ImGui::SeparatorText("Debug");
    ImGui::Text("NumPoints: %i", static_cast<int>(data.points));
    ImGui::Text("FPS: %f", data.framerate);
    
    if (ImGui::Button("Reload", ImVec2(-1, 0)))
        reload = true;
}

void SDRInstance::StartThread()
{
    PROFILE();
    StopThread();
    
    run = true;
    mainThread = std::thread([&]
    {
        while (run)
        {
            SDRContext context;
            reload = false;
            context.config = config;
            if (context.Init())
            {
                while (run && !reload)
                {
                    context.Frame();
                    threadLock.lock();
                    frameData = context.frame;
                    threadLock.unlock();
                }
            }
            context.Deinit();
        }
    });
}

void SDRInstance::StopThread()
{
    PROFILE();
    if (!run)
        return;
    run = false; 
    if (mainThread.joinable())
        mainThread.join();
}

bool SDRContext::Init()
{
    PROFILE();
    
    // Query available cameras
    availableCameras.clear();
    for (int i = 0; i < config.CameraQueryCount; ++i) 
    {
        cv::VideoCapture cap(i);
        if (cap.isOpened()) 
        {
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
    const float fovRad = config.CameraFOV * CV_PI / 180.0; // convert to radians
    const float fx = width / (2.0 * tan(fovRad / 2.0));
    const float fy = fx;
    const float cx = width/2.0;
    const float cy = height/2.0;
    K = (cv::Mat_<float>(3,3) << 
        fx, 0, cx,
       0, fy, cy,
       0, 0, 1);
    
    return true;
}

void SDRContext::Deinit()
{
    capL.release();
    capR.release();
}

void SDRContext::Frame() 
{
    PROFILE();
    
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
    float scale = Utility::Math::Clamp(config.Scale.Get(), 0.1f, 1.0f);
    cv::resize(grayL, graySmallL, cv::Size(), scale, scale);
    cv::resize(grayR, graySmallR, cv::Size(), scale, scale);

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
    const float focalLength = (K.at<float>(1, 1) + K.at<float>(2, 2)) / 2;
    cv::UMat fB(uDepth.size(), CV_32F, cv::Scalar(focalLength * baseline));
    cv::divide(fB, dispFloat, uDepth);

    // Mask invalid disparities
    cv::UMat mask;
    cv::compare(dispFloat, 0.0f, mask, cv::CMP_LE);
    uDepth.setTo(cv::Scalar(0.0f), mask);

    // 7. Feature detection (downscaled)
    if (featureUpdateTimer.Ellapsed() > config.FeatureUpdateFrequency || 
        pointsPrev.size() < config.FeatureUpdateMinPoints)
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
    CV_Assert(uDepth.type() == CV_32F);
    const auto depthMat = uDepth.getMat(cv::ACCESS_READ);
    for (size_t i = 0; i < pointsCurr.size(); ++i)
    {
        if (!status[i])
            continue;
        
        int x = static_cast<int>(pointsCurr[i].x);
        int y = static_cast<int>(pointsCurr[i].y);

        if (x < 0 || y < 0 || x >= depthMat.cols || y >= depthMat.rows)
            continue;

        float d = depthMat.at<float>(y, x);
        if (d <= 0.0f)
            continue;

        const float cx = K.at<float>(0,2);
        const float cy = K.at<float>(1,2);
        const float fx = K.at<float>(0,0);
        const float fy = K.at<float>(1,1);

        const float X = (x - cx) * d / fx;
        const float Y = (y - cy) * d / fy;
        const float Z = d;

        objectPoints.emplace_back(X, Y, Z);
        imagePoints.push_back(pointsCurr[i]);
    }
    
    // 10. Solve PnP for camera pose
    if (objectPoints.size() >= 6)
    {
        bool extrinsic = !rvec.empty() && !tvec.empty();
        if (cv::solvePnP(objectPoints, imagePoints, K, cv::Mat(), rvec, tvec, extrinsic, cv::SOLVEPNP_EPNP))
        {
            cv::Mat rotation;
            cv::Rodrigues(rvec, rotation);
            
            frame.transform.SetPosition({ 
                static_cast<float>(tvec.at<double>(0)), 
                static_cast<float>(tvec.at<double>(1)), 
                static_cast<float>(tvec.at<double>(2)) 
            });
            frame.transform.elements[0][0] = static_cast<float>(rotation.at<double>(0, 0)); 
            frame.transform.elements[0][1] = static_cast<float>(rotation.at<double>(0, 1)); 
            frame.transform.elements[0][2] = static_cast<float>(rotation.at<double>(0, 2)); 
            frame.transform.elements[1][0] = static_cast<float>(rotation.at<double>(1, 0)); 
            frame.transform.elements[1][1] = static_cast<float>(rotation.at<double>(1, 1)); 
            frame.transform.elements[1][2] = static_cast<float>(rotation.at<double>(1, 2)); 
            frame.transform.elements[2][0] = static_cast<float>(rotation.at<double>(2, 0)); 
            frame.transform.elements[2][1] = static_cast<float>(rotation.at<double>(2, 1)); 
            frame.transform.elements[2][2] = static_cast<float>(rotation.at<double>(2, 2)); 
        }
    }
    
    cv::namedWindow("Left", cv::WINDOW_NORMAL);
    cv::namedWindow("Right", cv::WINDOW_NORMAL);
    cv::namedWindow("Depth", cv::WINDOW_NORMAL | cv::WINDOW_OPENGL);
    if (config.Preview)
    {
        // 11. Draw tracking
        for (size_t i = 0; i < pointsCurr.size(); ++i)
        {
            CHECK_CONTINUE(!status[i]);
            cv::Point2f pt = pointsCurr[i] * (1.0f / scale); // scale up
            cv::circle(frameL, pt, 3, cv::Scalar(0, 255, 0), -1);
            cv::circle(frameR, pt, 3, cv::Scalar(0, 255, 0), -1);
        }
        
        // 12. Display debug
        cv::imshow("Left", frameL);
        cv::imshow("Right", frameR);
        cv::imshow("Depth", uDepth);
    }
    
    cv::waitKey(1);
    
    pointsPrev = pointsCurr;
    graySmallL.copyTo(prevGraySmall);
    
    frame.points = pointsCurr.size();
    frame.framerate = 1.0f / static_cast<float>(frameTimer.Ellapsed());
    frameTimer = {};
}
