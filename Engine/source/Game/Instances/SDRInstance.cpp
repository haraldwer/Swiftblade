#include "SDRInstance.h"

#include "opencv2/core/ocl.hpp"
#include "Utility/EditTransform.h"

void SDRInstance::Init()
{
    PROFILE();
    
    Instance::Init();
    
    LOG("OpenGL support: ", cv::getBuildInformation());
    
    cv::setUseOptimized(true);
    
    // Check OpenCL availability
    CHECK_RETURN_LOG(!cv::ocl::haveOpenCL(), "OpenCL not available");
    cv::ocl::setUseOpenCL(true);
    
    int threadCount = cv::getNumberOfCPUs();
    int numThreads = Utility::Math::Max(1, threadCount * 3 / 4);
    cv::setNumThreads(threadCount);
    LOG("CV using num threads: ", numThreads);
    
    StartThread();
}

void SDRInstance::Deinit()
{
    PROFILE();
    
    StopThread();
    Instance::Deinit();
}

void SDRInstance::Frame()
{
    PROFILE();
    
    Instance::Frame();
    
    frameData.SwapBack();
    SDR::FrameData data = frameData.Back();
    // Render something?
    
    int space = 0;
    int operation = 0;
    bool snap = false;
    Engine::EditTransform(data.transform, space, operation, snap);
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

    frameData.SwapBack();
    const auto data = frameData.Back();
    
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
            SDR::Context context;
            reload = false;
            context.config = config;
            if (context.Init())
            {
                while (run && !reload)
                {
                    context.Frame();
                    frameData.Front() = context.frame; 
                    frameData.SwapFront();
                }
            }
            context.Deinit();
        }
    });
}

void SDRInstance::StopThread()
{
    PROFILE();
    CHECK_RETURN(!run);
    if (mainThread.joinable())
        mainThread.join();
    run = false; 
}
