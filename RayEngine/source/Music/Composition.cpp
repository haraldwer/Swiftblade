#include "Composition.h"

void Composition::Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const
{
    for (auto& track : Tracks.Get())
    {
        // TODO: Offset frame with track start
        track.Fill(InFrame, InSamples, InFrames, InSampleRate);
    }

    Effects.Get().Fill(InFrame, InSamples, InFrames, InSampleRate);
}

