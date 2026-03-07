#pragma once

#include "SDRDepth.h"
#include "SDRTracking.h"
#include "opencv2/videoio.hpp"

namespace SDR
{
    struct Config : PropertyOwner<Config>
    {
        PROPERTY_C(int, StereoNumDisparities, 64);
        PROPERTY_C(int, StereoBlockSize, 5);
        
        PROPERTY_C(float, FeatureUpdateFrequency, 5.0f);
        PROPERTY_C(float, FeatureUpdateMinPoints, 20);
        PROPERTY_C(int, FeatureMaxCorners, 150);
        PROPERTY_C(float, FeatureQualityLevel, 0.01f);
        PROPERTY_C(float, FeatureMinDistance, 10.0f);
        
        PROPERTY_C(Vec2I, OpticalFlowWindowSize, Vec2I(10));
        PROPERTY_C(int, OpticalFlowLevel, 2);
        
        PROPERTY_C(bool, Preview, true);
        
        PROPERTY_C(float, FocalLength, 0.5f);
        PROPERTY_C(float, StereoCameraDistance, 4.0f);
        PROPERTY_C(float, CameraFOV, 95.5f);
        PROPERTY_C(Vec2I, CameraDefaultSize, Vec2I(640, 480))
        PROPERTY_C(int, CameraQueryCount, 10);
        PROPERTY_C(int, CameraPortStart, 6767);
        PROPERTY_C(int, CameraPortCount, 10);
    };

    struct FrameData
    {
        Mat4F transform;
        // TODO: Depth data
        
        // Debug
        bool left = false;
        bool right = false;
        int points = 0;
        float framerate = 0.0f;
    };

    struct ContextData
    {
        cv::Mat K;
        cv::Mat Left; 
        cv::Mat Right; 
        cv::Mat GrayLeft; 
        cv::Mat PrevGrayLeft;
        cv::Mat GrayRight; 
        cv::Mat Depth;
        
        cv::Mat DisparityVisual;
        cv::Mat Disparity;
    };
    
    struct Context
    { 
        bool Init();
        void Deinit();
        void Frame();
        
        cv::VideoCapture capL, capR;
        
        Config config;
        ContextData data;
        FrameData frame;
        Utility::Timer frameTimer;
        
        Tracking tracking;
        Depth depth;
    };
}

