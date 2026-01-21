#pragma once

#include "Composition.h"
#include "ToneLibrary.h"
#include "Interface/Audio.h"
#include "Utility/Collections/SwapBuffer.h"

namespace Music
{
    struct GenData : PropertyOwner<GenData>
    {
        PROPERTY(ToneLibrary, Tones);
        PROPERTY(Composition, Comp);
    };
    
    struct MusicData
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
        Utility::SwapBuffer<MusicData> audioData;
        
    private:

        Config config;
        Audio::Stream stream;
        uint32 frame = 0;
    };
}
