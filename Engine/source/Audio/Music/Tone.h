#pragma once

#include "Core/Property/Expressions/Expression.h"
#include "ToneTiming.h"

namespace Music
{
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
        bool Edit(const String &InName, uint32 InOffset) override;
        bool Edit();
        void Plot(const String& InOverlay);

        PROPERTY_D(String, Name, "Unnamed");
        
    private:
        PROPERTY_D(Expression, Frequency, "octaves(FREQ, 3)");
        PROPERTY_D(Expression, Volume, "min(smooth(START, 0, 0.05), pow(END + 0.05, 4))");

        void UpdateTonePreview(const ToneTiming& InTiming);
        static void UpdatePreview(const Expression& InExpr, const ToneTiming& InTiming, Vector<float>& InPreview);
        static float Evaluate(const Expression& InFunc, const ToneTiming& InTiming, uint64 InFrame);
        static void Declare(Expression& InFunc);

        Vector<float> freqPreview = {};
        Vector<float> volumePreview = {};
        Vector<float> tonePreview = {};
    };
}

