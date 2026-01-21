#pragma once

namespace Music
{
    struct ToneTiming
    {
        uint32 sampleRate = 48000; // Samples per second
        uint32 beatLength = 48000; // How long the beat is (1 sec)
        uint32 noteLength = 48000 / 4; // How long the note is
        float pitch = 440.0f; // The note (A)

        float GetGlobal(const uint64 InFrame) const
        {
            return static_cast<float>(static_cast<double>(InFrame) / static_cast<double>(sampleRate));
        }
        
        float GetFrequency(const uint64 InFrame) const
        {
            uint64 noteFrame = InFrame % noteLength;
            double ellapsedTime = static_cast<double>(noteFrame) / static_cast<double>(sampleRate);
            return static_cast<float>(ellapsedTime * pitch) * PI_FLOAT * 2.0f;
        }
        
        float GetNote(const uint64 InFrame) const
        {
            uint64 noteFrame = InFrame % noteLength;
            return static_cast<float>(static_cast<double>(noteFrame) / static_cast<double>(noteLength));
        }
        
        float GetMelody(uint64 InFrame) const
        {
            return 0.0f;
        }
        
        float GetStart(const uint64 InFrame) const
        {
            uint64 noteFrame = InFrame % noteLength;
            return static_cast<float>(static_cast<double>(noteFrame) / static_cast<double>(noteLength));
        }
        
        float GetEnd(const uint64 InFrame) const
        {
            uint64 noteFrame = InFrame % noteLength;
            uint64 noteEndFrame = noteLength - noteFrame;
            return static_cast<float>(static_cast<double>(noteEndFrame) / static_cast<double>(noteLength));
        }
    };
}

