#include "SDRDepth.h"

#include "SDRContext.h"

void SDR::Depth::Frame(FrameData& InOutFrame, ContextData& InOutData, const Config& InConfig)
{
    PROFILE();
    
    if (!stereo)
    {
        PROFILE_NAMED("Create stereo")
        stereo = cv::StereoSGBM::create(0, InConfig.StereoNumDisparities, InConfig.StereoBlockSize);
        
        // 3. Fine-tuning Penalties (The "Secret Sauce")
        int channels = InOutData.GrayLeft.channels();
        stereo->setP1(8 * channels * InConfig.StereoBlockSize * InConfig.StereoBlockSize);
        stereo->setP2(32 * channels * InConfig.StereoBlockSize * InConfig.StereoBlockSize);
    
        // 4. Post-processing filters
        stereo->setUniquenessRatio(0);
        stereo->setDisp12MaxDiff(-1);
        stereo->setSpeckleWindowSize(0);
        //stereo->setSpeckleRange(32);
        //stereo->setMode(cv::StereoSGBM::MODE_SGBM_3WAY); // Faster than standard SGBM
    }
    
    {
        PROFILE_NAMED("Compute disparity")
        InOutData.Disparity = cv::Mat();
        stereo->compute(InOutData.GrayLeft, InOutData.GrayRight, InOutData.Disparity);
        InOutData.DisparityVisual = cv::Mat();
        InOutData.Disparity.convertTo(InOutData.DisparityVisual, CV_32F, 1.0 / 16.0);// BM scale
    }
    
    {
        PROFILE_NAMED("Calculate depth")
        const float baseline = InConfig.StereoCameraDistance;
        float fx = InOutData.K.at<float>(0,0);
        float fy = InOutData.K.at<float>(1,1);
        const float focalLength = (fx + fy) * 0.5f;
        InOutData.Depth = (focalLength * baseline) / InOutData.DisparityVisual;
    }
    
    // TODO: Copy depth to Frame
    // So that it can be used in renderer
}
