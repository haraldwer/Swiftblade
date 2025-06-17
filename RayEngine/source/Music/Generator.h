#pragma once
#include "Collections/SwapBuffer.h"

struct AudioStream;

namespace Audio
{
    struct GenData : PropertyOwner<GenData>
    {
        
    };
    
    struct AudioData
    {
        Vector<float> Data;
    };

    struct Config : BaseConfig<GenData>
    {
        PROPERTY_D(float, Master, 0.005f);
        PROPERTY_D(int, BufferSize, 256);
        PROPERTY_D(int, SampleRate, 48000);
        PROPERTY_D(int, SampleSize, 32);
        PROPERTY_D(int, Channels, 1);

        String Name() const override { return "Music"; }
    };
    
    class Generator : public Utility::Singelton<Generator, true>
    {
    public:
        void Init(Config InConfig);
        void Deinit();
        
        Utility::SwapBuffer<GenData> genData;
        Utility::SwapBuffer<AudioData> audioData;
        
    private:
        static void FillBuffer(void * buffer, unsigned int frames);
        
        AudioStream* stream = nullptr;
    };
}
