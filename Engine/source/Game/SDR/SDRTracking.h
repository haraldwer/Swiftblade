#pragma once

#include "opencv2/calib3d.hpp"
#include "opencv2/videoio.hpp"

namespace SDR
{
    struct FrameData;
    struct ContextData;
    struct Config;
    
    // Camera tracking in world-space
    class Tracking
    {
    public:
        void Frame(FrameData& InOutFrame, ContextData& InOutData, const Config& InConfig);
        
    private:
        
        Utility::Timer featureUpdateTimer;
        std::vector<cv::Point2f> pointsPrev, pointsCurr;
        std::vector<cv::Vec3f> objectPoints;
        std::vector<cv::Point2f> imagePoints;
        cv::Mat rvec, tvec;
        
        std::vector<uchar> status;
        std::vector<float> err;
        
    };
}
