#include "Effect.h"

void Effect::Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const
{
}

void EffectStack::Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const
{
    for (auto& eff : Effects.Get())
        eff.Fill(InFrame, InSamples, InFrames, InSampleRate);
}
