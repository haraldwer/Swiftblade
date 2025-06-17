#include "Generator.h"

#include "Math/Random.hpp"
#include "raylib.h"

void Audio::Generator::Init(Config InConfig)
{
    PROFILE_AU();
    
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
    SetAudioStreamVolume(*stream, InConfig.Master);
    SetAudioStreamCallback(*stream, &FillBuffer);
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

    auto& audioData = Get().audioData; 
    auto& data = audioData.Front().Data;
    data.clear();
    data.resize(frames, 0);

    float a = 440;
    float sampleRate = 48000;
    float freq = PI * 2.0f;
    
    static Random rand;
    const auto samples = static_cast<float*>(buffer);
    for (unsigned int i = 0; i < frames; i++)
    {
        float val = sinf(a * freq * static_cast<float>(i) / sampleRate);
        samples[i] = val;
        data[i] = val;
    }

    audioData.SwapFront();
}
