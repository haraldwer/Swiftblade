#include "Generator.h"

#include "Math/Random.hpp"
#include "raylib.h"

void Fill(void* InBuffer, const unsigned int InFrames)
{
    Audio::Generator::Get().FillBuffer(InBuffer, InFrames);
}

void Audio::Generator::Init(Config InConfig)
{
    PROFILE_AU();

    config = InConfig;
    
    if (IsAudioDeviceReady())
        CloseAudioDevice();
    InitAudioDevice();

    if (stream)
    {
        UnloadAudioStream(*stream);
        delete stream;
    }
    
    stream = new AudioStream();
    *stream = LoadAudioStream(InConfig.SampleRate, InConfig.SampleSize, InConfig.Channels);

    SetAudioStreamBufferSizeDefault(InConfig.BufferSize);
    SetAudioStreamCallback(*stream, &Fill);
    PlayAudioStream(*stream);
}

void Audio::Generator::Deinit() 
{
    PROFILE_AU();
    if (stream)
    {
        UnloadAudioStream(*stream);
        delete stream;
        stream = nullptr;
    }
    CloseAudioDevice();
}

void Audio::Generator::FillBuffer(void* buffer, unsigned int frames)
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
