#pragma once
#include "Effect.h"
#include "Track.h"

class Composition : public PropertyOwner<Composition>
{
public:
    void Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const;

private:
    PROPERTY(Vector<Track>, Tracks);
    PROPERTY(EffectStack, Effects);
};
