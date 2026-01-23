#include "Generator.h"

#include "Audio/Interface/Interface.h"

void Fill(void* InBuffer, const unsigned int InFrames)
{
    Music::Generator::Get().FillBuffer(InBuffer, InFrames);
}

void Music::Generator::Init(Config InConfig)
{
    PROFILE_AU();

    config = InConfig;
    
    if (Audio::IsDeviceReady())
        Audio::CloseDevice();
    Audio::InitDevice();

    Audio::UnloadStream(stream);
    stream = Audio::LoadStream(InConfig.SampleRate, InConfig.SampleSize, InConfig.Channels);
    Audio::SetSteamBufferSize(InConfig.BufferSize);
    Audio::SetStreamCallback(stream, &Fill);
    Audio::PlayStream(stream);
}

void Music::Generator::Deinit() 
{
    PROFILE_AU();
    Audio::UnloadStream(stream);
    stream = {};
    Audio::CloseDevice();
}

void Music::Generator::FillBuffer(void* buffer, unsigned int frames)
{
    PROFILE_AU();

    const auto samples = static_cast<float*>(buffer);
    memset(samples, 0, sizeof(float) * frames); // Reset buffer

    // TODO: Get all active compositions and their active effects?
    auto& in = genData.SwapBack();
    in.Comp.Get().Fill(frame, samples, frames, config.SampleRate.Get());
    
    for (unsigned int i = 0; i < frames; i++)
        samples[i] = Utility::Math::Clamp(samples[i] * config.Master.Get(), -1.0f, 1.0f);

    auto& out = audioData.Front().Data;
    out.clear();
    out.resize(frames, 0);
    memcpy(out.data(), samples, sizeof(float) * frames);
    audioData.SwapFront();
}
