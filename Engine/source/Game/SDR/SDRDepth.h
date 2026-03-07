#pragma once

#include "opencv2/calib3d.hpp"
#include "opencv2/videoio.hpp"

namespace SDR
{
    struct FrameData;
    struct ContextData;
    struct Config;
    class Depth
    {
    public:
        void Frame(FrameData& InOutFrame, ContextData& InOutData, const Config& InConfig);
        
    private:
        cv::Ptr<cv::StereoSGBM> stereo;
    };
}
