#pragma once

class Effect : public PropertyOwner<Effect>
{
public:
    void Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const;
};

class EffectStack : public PropertyOwner<EffectStack>
{
public:
    void Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const;

private:
    PROPERTY(Vector<Effect>, Effects);
};
