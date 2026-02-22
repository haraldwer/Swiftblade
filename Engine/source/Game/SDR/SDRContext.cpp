#include "SDRContext.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/legacy/constants_c.h"
#include "opencv2/opencv_modules.hpp"

String ConstructPipeline(int InPort)
{
    return std::format(R"(
        udpsrc port={0} \
        caps="application/x-rtp,media=video,clock-rate=90000,encoding-name=H264,payload=96" ! \
        rtph264depay ! \
        avdec_h264 max-threads=1 ! \
        videoconvert ! \
        appsink name=sink{0}
    )", InPort);
}

bool SDR::Context::Init()
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
    
    // Create cameras
    int availableIndex = 0;
    if (config.CameraLeft != 0)
        capL = cv::VideoCapture(ConstructPipeline(config.CameraLeft), cv::CAP_GSTREAMER);
    else
    {
        capL = cv::VideoCapture(availableCameras.at(availableIndex));
        availableIndex++;
    }
        
    if (config.CameraLeft != 0)
        capR = cv::VideoCapture(ConstructPipeline(config.CameraRight), cv::CAP_GSTREAMER);
    else
    {
        capR = cv::VideoCapture(availableCameras.at(availableIndex));
        availableIndex++;
    }
    
    frame.left = capL.isOpened();
    frame.right = capR.isOpened();
    
    CHECK_RETURN_LOG(!capL.isOpened() || !capL.isOpened(), "Failed to open cameras", false);
    
    capL.set(cv::CAP_PROP_BUFFERSIZE, 1);
    capL.set(cv::CAP_PROP_FPS, config.CameraFPS);
    capR.set(cv::CAP_PROP_BUFFERSIZE, 1);
    capR.set(cv::CAP_PROP_FPS, config.CameraFPS);
    
    // Set capture resolution
    float scale = Utility::Math::Clamp(config.Scale.Get(), 0.1f, 1.0f);
    int heightL = capL.get(CV_CAP_PROP_FRAME_HEIGHT);
    int widthL = capL.get(CV_CAP_PROP_FRAME_WIDTH);
    int heightR = capR.get(CV_CAP_PROP_FRAME_HEIGHT);
    int widthR = capR.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = Utility::Math::Min(heightL, heightR);
    int width = Utility::Math::Min(widthL, widthR);
    capL.set(CV_CAP_PROP_FRAME_HEIGHT, static_cast<int>(height * scale));
    capL.set(CV_CAP_PROP_FRAME_WIDTH, static_cast<int>(width * scale));
    capR.set(CV_CAP_PROP_FRAME_HEIGHT, static_cast<int>(height * scale));
    capR.set(CV_CAP_PROP_FRAME_WIDTH, static_cast<int>(width * scale));
    
    // Calibrate
    const float fovRad = config.CameraFOV * CV_PI / 180.0; // convert to radians
    const float fx = width / (2.0 * tan(fovRad / 2.0));
    const float fy = fx;
    const float cx = width/2.0;
    const float cy = height/2.0;
    data.K = (cv::Mat_<float>(3,3) << 
        fx, 0, cx,
       0, fy, cy,
       0, 0, 1);
    
    return true;
}

void SDR::Context::Deinit()
{
    PROFILE();
    
    capL.release();
    capR.release();
}

void SDR::Context::Frame() 
{
    PROFILE();
    
    CHECK_RETURN(!capL.isOpened() || !capR.isOpened());

    // Capture frames
    capL >> data.Left;
    capR >> data.Right;
    CHECK_RETURN(data.Left.empty() || data.Right.empty());
    
    // TODO: Possibly some post processing
    
    // Convert to grayscale
    cv::cvtColor(data.Left, data.GrayLeft, cv::COLOR_BGR2GRAY);
    cv::cvtColor(data.Right, data.GrayRight, cv::COLOR_BGR2GRAY);

    if (data.PrevGrayLeft.empty())
        data.GrayLeft.copyTo(data.PrevGrayLeft);
    
    depth.Frame(frame, data, config);
    tracking.Frame(frame, data, config);
    data.GrayLeft.copyTo(data.PrevGrayLeft);
    
    // Display debug
    cv::namedWindow("Left", cv::WINDOW_NORMAL | cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Right", cv::WINDOW_NORMAL | cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Depth", cv::WINDOW_NORMAL | cv::WINDOW_OPENGL | cv::WINDOW_AUTOSIZE);
    if (config.Preview)
    {
        PROFILE();
        cv::imshow("Left", data.Left);
        cv::imshow("Right", data.Right);
        cv::imshow("Depth", data.Depth);
    }
    cv::waitKey(1);
    
    frame.framerate = 1.0f / static_cast<float>(frameTimer.Ellapsed());
    frameTimer = {};
}
