#pragma once
#include "Collections/SwapBuffer.h"
#include "Composition.h"
#include "Tone.h"

struct AudioStream;

namespace Audio
{
    typedef Map<String, Tone> ToneMap; 
    
    struct GenData : PropertyOwner<GenData>
    {
        PROPERTY(ToneMap, Tones);
        PROPERTY(Composition, Comp);
    };
    
    struct AudioData
    {
        Vector<float> Data;
    };

    struct Config : BaseConfig<GenData>
    {
        PROPERTY_D(float, Master, 0.0f);
        PROPERTY_D(int, BufferSize, 1024);
        PROPERTY_D(uint32, SampleRate, 48000);
        PROPERTY_D(uint32, SampleSize, 32);
        PROPERTY_D(uint32, Channels, 1);

        String Name() const override { return "Music"; }
    };
    
    class Generator : public Utility::Singelton<Generator, true>
    {
    public:
        void Init(Config InConfig);
        void Deinit();
        
        void FillBuffer(void * buffer, unsigned int frames);
        
        Utility::SwapBuffer<GenData> genData;
        Utility::SwapBuffer<AudioData> audioData;
        
    private:

        Config config;
        AudioStream* stream = nullptr;
        uint32 frame = 0;
    };
}
