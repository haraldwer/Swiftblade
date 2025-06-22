#pragma once

#include "Property/Expression.h"
#include "ToneTiming.h"

constexpr int TONE_PLOT_ACCURACY = 1024;

enum class TimingVars : uint8
{
    FREQ,
    NOTE,
    MELODY,
    GLOBAL,
    START,
    END
};

class Tone : public PropertyOwner<Tone>
{
public:
    Tone();
    float Evaluate(const ToneTiming& InTiming, uint64 InFrame) const;
    bool Edit(const String& InName, uint32 InOffset) override;

private:
    PROPERTY_D(Expression, Frequency, "octaves(FREQ, 3)");
    PROPERTY_D(Expression, Volume, "min(smooth(START, 0, 0.05), pow(END + 0.05, 4))");

    static float Evaluate(const Expression& InFunc, const ToneTiming& InTiming, uint64 InFrame);
    static void Declare(Expression& InFunc);

    Vector<float> freqPreview = {};
    Vector<float> volumePreview = {};
    Vector<float> tonePreview = {};
};
