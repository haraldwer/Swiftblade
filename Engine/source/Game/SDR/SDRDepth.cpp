#include "SDRDepth.h"

#include "SDRContext.h"

void SDR::Depth::Frame(FrameData& InOutFrame, ContextData& InOutData, const Config& InConfig)
{
    PROFILE();
    
    if (!stereo)
    {
        PROFILE_NAMED("Create stereo")
        stereo = cv::StereoBM::create(InConfig.StereoNumDisparities, InConfig.StereoBlockSize);
        
    }
    
    {
        PROFILE_NAMED("Compute disparity")
        stereo->compute(InOutData.GrayLeft, InOutData.GrayRight, Disparity);
        Disparity.convertTo(DisparityFloat, CV_32F, 1.0 / 16.0);// BM scale
    }
    
    {
        PROFILE_NAMED("Calculate depth")
        const float baseline = InConfig.StereoCameraDistance;
        const float focalLength = (InOutData.K.at<float>(1, 1) + InOutData.K.at<float>(2, 2)) / 2;
        if (FocalBase.size() != Disparity.size())
            FocalBase.create(Disparity.size(), CV_32F);
        FocalBase.setTo(cv::Scalar(focalLength * baseline));
        cv::divide(FocalBase, DisparityFloat, InOutData.Depth);
    }
    
    // TODO: Copy depth to Frame
    // So that it can be used in renderer
}
