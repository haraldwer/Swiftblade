#pragma once

#include "Collections/SwapBuffer.h"
#include "Instance/Instance.h"
#include "SDR/SDRContext.h"

class SDRInstance : public Engine::Instance, public Debug::Panel
{
    CLASS_INFO(SDRInstance, Engine::Instance);
    
public:
    void Init() override;
    void Deinit() override;
    void Frame() override;
    
    void DrawPanel() override;
    String PanelName() const override { return "Scene Depth Reconstruction"; }
    
private:
    void StartThread();
    void StopThread();
    
    SDR::Config config;
    std::thread mainThread;
    Utility::SwapBuffer<SDR::FrameData> frameData;
    bool run = false;
    bool reload = false;
};
