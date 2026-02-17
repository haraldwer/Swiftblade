#include "SDRTracking.h"
#include "SDRContext.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video/tracking.hpp"


void SDR::Tracking::Frame(FrameData& InOutFrame, ContextData& InOutData, const Config &InConfig)
{
    PROFILE();
    
    // Feature detection (downscaled)
    if (featureUpdateTimer.Ellapsed() > InConfig.FeatureUpdateFrequency || 
        pointsPrev.size() < InConfig.FeatureUpdateMinPoints)
    {
        PROFILE_NAMED("Find features")
        cv::goodFeaturesToTrack(InOutData.GrayLeft, pointsPrev, InConfig.FeatureMaxCorners, InConfig.FeatureQualityLevel, InConfig.FeatureMinDistance);
        featureUpdateTimer = {};
    }

    // LK optical flow (downscaled)
    {
        PROFILE_NAMED("Calculate optical flow")
        auto size = InConfig.OpticalFlowWindowSize.Get();
        cv::calcOpticalFlowPyrLK(InOutData.PrevGrayLeft, InOutData.GrayLeft, pointsPrev, pointsCurr, status, err, { size.x, size.y }, InConfig.OpticalFlowLevel.Get());
    }

    // Prepare 3D points for PnP (sparse)
    objectPoints.clear();
    imagePoints.clear();
    objectPoints.reserve(pointsCurr.size());
    imagePoints.reserve(pointsCurr.size());
    CV_Assert(InOutData.Depth.type() == CV_32F);
    const auto depthMat = InOutData.Depth.getMat(cv::ACCESS_READ);
    {
        PROFILE_NAMED("Project points");
        for (size_t i = 0; i < pointsCurr.size(); ++i)
        {
            CHECK_CONTINUE(!status[i]);
            const int x = static_cast<int>(pointsCurr[i].x);
            const int y = static_cast<int>(pointsCurr[i].y);
            CHECK_CONTINUE(x < 0 || y < 0 || x >= depthMat.cols || y >= depthMat.rows);
            float d = depthMat.at<float>(y, x);
            CHECK_CONTINUE(d <= 0.0f);

            const float cx = InOutData.K.at<float>(0,2);
            const float cy = InOutData.K.at<float>(1,2);
            const float fx = InOutData.K.at<float>(0,0);
            const float fy = InOutData.K.at<float>(1,1);

            const float X = (x - cx) * d / fx;
            const float Y = (y - cy) * d / fy;
            const float Z = d;

            objectPoints.emplace_back(X, Y, Z);
            imagePoints.push_back(pointsCurr[i]);
        }
    }
    
    // Solve PnP for camera pose
    if (objectPoints.size() >= 6)
    {
        PROFILE_NAMED("Solve pose");
        bool extrinsic = !rvec.empty() && !tvec.empty();
        if (cv::solvePnP(objectPoints, imagePoints, InOutData.K, cv::Mat(), rvec, tvec, extrinsic, cv::SOLVEPNP_EPNP))
        {
            PROFILE_NAMED("Apply pose");
            cv::Mat rotation;
            cv::Rodrigues(rvec, rotation);
            
            InOutFrame.transform.SetPosition({ 
                static_cast<float>(tvec.at<double>(0)), 
                static_cast<float>(tvec.at<double>(1)), 
                static_cast<float>(tvec.at<double>(2)) 
            });
            InOutFrame.transform.elements[0][0] = static_cast<float>(rotation.at<double>(0, 0)); 
            InOutFrame.transform.elements[0][1] = static_cast<float>(rotation.at<double>(0, 1)); 
            InOutFrame.transform.elements[0][2] = static_cast<float>(rotation.at<double>(0, 2)); 
            InOutFrame.transform.elements[1][0] = static_cast<float>(rotation.at<double>(1, 0)); 
            InOutFrame.transform.elements[1][1] = static_cast<float>(rotation.at<double>(1, 1)); 
            InOutFrame.transform.elements[1][2] = static_cast<float>(rotation.at<double>(1, 2)); 
            InOutFrame.transform.elements[2][0] = static_cast<float>(rotation.at<double>(2, 0)); 
            InOutFrame.transform.elements[2][1] = static_cast<float>(rotation.at<double>(2, 1)); 
            InOutFrame.transform.elements[2][2] = static_cast<float>(rotation.at<double>(2, 2)); 
        }
    }
    
    if (InConfig.Preview)
    {
        PROFILE_NAMED("Preview");
        for (size_t i = 0; i < pointsCurr.size(); ++i)
           if (status[i]) cv::circle(InOutData.Left, pointsCurr[i], 3, cv::Scalar(0, 255, 0), -1);
    }
    
    pointsPrev = pointsCurr;
    InOutFrame.points = pointsCurr.size();
}

