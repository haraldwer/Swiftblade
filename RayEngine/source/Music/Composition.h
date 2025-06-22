#pragma once
#include "Track.h"

class Composition : public PropertyOwner<Track>
{
public:
    void Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const;

private:
    PROPERTY(Vector<Track>, Tracks);
    //PROPERTY(EffectStack, Effects);
};
