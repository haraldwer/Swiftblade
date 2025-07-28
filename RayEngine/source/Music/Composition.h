#pragma once
#include "Effect.h"
#include "Track.h"

class Composition : public PropertyOwner<Composition>
{
public:
    void Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const;
    bool Edit(const String &InName, uint32 InOffset) override;

private:
    PROPERTY(Vector<Track>, Tracks);
    PROPERTY(EffectStack, Effects);
};
