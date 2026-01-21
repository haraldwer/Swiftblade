#include "Effect.h"

void Music::Effect::Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const
{
}

void Music::EffectStack::Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const
{
    for (auto& eff : Effects.Get())
        eff.Fill(InFrame, InSamples, InFrames, InSampleRate);
}
