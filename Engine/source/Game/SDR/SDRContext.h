#pragma once

#include "SDRDepth.h"
#include "SDRTracking.h"
#include "opencv2/calib3d.hpp"
#include "opencv2/videoio.hpp"

namespace SDR
{
    struct Config : PropertyOwner<Config>
    {
        PROPERTY_C(int, CameraQueryCount, 10);
        
        PROPERTY_C(int, StereoNumDisparities, 64);
        PROPERTY_C(int, StereoBlockSize, 5);
        
        PROPERTY_C(float, FeatureUpdateFrequency, 5.0f);
        PROPERTY_C(float, FeatureUpdateMinPoints, 20);
        PROPERTY_C(int, FeatureMaxCorners, 150);
        PROPERTY_C(float, FeatureQualityLevel, 0.01f);
        PROPERTY_C(float, FeatureMinDistance, 10.0f);
        
        PROPERTY_C(Vec2I, OpticalFlowWindowSize, Vec2I(10));
        PROPERTY_C(int, OpticalFlowLevel, 2);
        
        PROPERTY_C(float, Scale, 0.5f);
        PROPERTY_C(bool, Preview, true);
        
        PROPERTY_C(float, FocalLength, 0.5f);
        PROPERTY_C(float, StereoCameraDistance, 10.0f);
        PROPERTY_C(float, CameraFOV, 90.0f);
        PROPERTY_C(int, CameraFPS, 30);
        PROPERTY_C(int, CameraLeft, 6767);
        PROPERTY_C(int, CameraRight, 6768);
    };

    struct FrameData
    {
        Mat4F transform;
        // TODO: Depth data
        
        // Debug
        int numCameras = 0;
        bool left = false;
        bool right = false;
        int points = 0;
        float framerate = 0.0f;
    };

    struct ContextData
    {
        cv::Mat K;
        cv::UMat Left; 
        cv::UMat Right; 
        cv::UMat GrayLeft; 
        cv::UMat PrevGrayLeft;
        cv::UMat GrayRight; 
        cv::UMat Depth; 
    };
    
    struct Context
    { 
        bool Init();
        void Deinit();
        void Frame();
        
        std::vector<int> availableCameras;
        cv::VideoCapture capL, capR;
        
        Config config;
        ContextData data;
        FrameData frame;
        Utility::Timer frameTimer;
        
        Tracking tracking;
        Depth depth;
    };
}

