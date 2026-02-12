#pragma once

#include "Instance/Instance.h"

#include "opencv2/calib3d.hpp"
#include "opencv2/videoio.hpp"

struct SDRConfig : PropertyOwner<SDRConfig>
{
    PROPERTY_C(int, CameraQueryCount, 10);
    
    PROPERTY_C(int, StereoNumDisparities, 128);
    PROPERTY_C(int, StereoBlockSize, 15);
    
    PROPERTY_C(float, FeatureUpdateFrequency, 5.0f);
    PROPERTY_C(int, FeatureMaxCorners, 200);
    PROPERTY_C(float, FeatureQualityLevel, 0.01f);
    PROPERTY_C(float, FeatureMinDistance, 10.0f);
    
    PROPERTY_C(float, Scale, 0.5f);
    PROPERTY_C(float, FocalLength, 0.5f);
    PROPERTY_C(float, StereoCameraDistance, 0.5f);
    PROPERTY_C(float, CameraFOV, 0.5f);
    
};

struct SDRFrameData
{
    int numCameras = 0;
    bool left = false;
    bool right = false;
    int points = 0;
};

struct SDRContext
{ 
    bool Init();
    void Deinit();
    void Frame();
    
    std::vector<int> availableCameras;
    cv::Ptr<cv::StereoBM> stereo;
    cv::VideoCapture capL, capR;
    
    cv::Mat K, frameL, frameR, grayL, grayR, graySmallL, graySmallR, prevGray, prevGraySmall, disparity, points3D;
    cv::UMat uLeft, uRight, uDisp, uDepth;
    std::vector<cv::Point2f> pointsPrev, pointsCurr;
    std::vector<cv::Vec3f> objectPoints;
    std::vector<cv::Point2f> imagePoints;
    
    std::vector<uchar> status;
    std::vector<float> err;
    
    Utility::Timer featureUpdateTimer;
    
    SDRConfig config;
    SDRFrameData frame;
};

class SDRInstance : public Engine::Instance, public Debug::Panel
{
    CLASS_INFO(SDRInstance, Engine::Instance);
    
public:

    void Init() override;
    void Deinit() override;
    
    void DrawPanel() override;
    String PanelName() const override { return "Depth Estimation"; }
    
private:
    void StartThread();
    void StopThread();
    
    SDRConfig config;
    SDRFrameData frameData;
    
    std::mutex threadLock;
    std::thread mainThread;
    bool run = false;
};
