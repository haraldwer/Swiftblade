#include "Track.h"

void Track::Add(const Note& InNote)
{
    
}

void Track::Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const
{
    Effects.Get().Fill(InFrame, InSamples, InFrames, InSampleRate);
}
