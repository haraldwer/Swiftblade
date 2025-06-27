#include "Tone.h"

#include "ImGui/imgui_custom.h"

#define TONE_DECLARE(func, var) func.Declare(static_cast<uint8>(TimingVars::var), #var);
#define TONE_SET(func, var, val) func.Set(static_cast<uint8>(TimingVars::var), val);

Tone::Tone()
{
    Declare(Frequency.Get());    
    Declare(Volume.Get());
}

float Tone::Evaluate(const ToneTiming& InTiming, const uint64 InFrame) const
{
    const float f = Evaluate(Frequency, InTiming, InFrame);
    const float v = Utility::Math::Clamp(Evaluate(Volume, InTiming, InFrame), 0.0f, 1.0f);
    return f * v;
}

bool Tone::Edit(const String& InName, const uint32 InOffset)
{
    bool result = false;

    ImGui::Text("%s", InName.c_str());
    
    ImGui::PushItemWidth(-FLT_MIN);
    constexpr ImVec2 plotSize = ImVec2(0, 50);

    ToneTiming previewTiming;
    previewTiming.sampleRate = TONE_PLOT_ACCURACY;
    previewTiming.beatLength = TONE_PLOT_ACCURACY;
    previewTiming.noteLength = TONE_PLOT_ACCURACY / 4;

    ImGui::SeparatorText("Frequency");
    const bool freqEdit = Frequency.Edit(InOffset);
    if (freqEdit || freqPreview.empty())
    {
        const ToneTiming freqTiming = previewTiming;
        freqPreview.resize(TONE_PLOT_ACCURACY);
        for (uint64 i = 0; i < TONE_PLOT_ACCURACY; i++)
            freqPreview[i] = Evaluate(Frequency, freqTiming, i);
    }
    ImGui::PlotLines("##Freq", freqPreview.data(), static_cast<int>(freqPreview.size()), 0, 0, FLT_MAX, FLT_MAX, plotSize);
    result |= freqEdit;
    
    ImGui::SeparatorText("Volume");
    const bool volEdit = Volume.Edit(InOffset);
    if (volEdit || volumePreview.empty())
    {
        const ToneTiming volTiming = previewTiming;
        volumePreview.resize(TONE_PLOT_ACCURACY);
        for (uint64 i = 0; i < TONE_PLOT_ACCURACY; i++)
            volumePreview[i] = Evaluate(Volume, volTiming, i);
    }
    ImGui::PlotLines("##Vol", volumePreview.data(), static_cast<int>(volumePreview.size()), 0, 0, FLT_MAX, FLT_MAX, plotSize);
    result |= volEdit;

    if (result || tonePreview.empty())
    {
        tonePreview.resize(TONE_PLOT_ACCURACY);
        for (uint64 i = 0; i < TONE_PLOT_ACCURACY; i++)
            tonePreview[i] = Utility::Math::Clamp(
                Evaluate(Frequency, previewTiming, i) *
                Evaluate(Volume, previewTiming, i),
                -1.0f,
                1.0f);
    }
    ImGui::SeparatorText("Tone");
    ImGui::PlotLines("##Tone", tonePreview.data(), static_cast<int>(tonePreview.size()), 0, 0, FLT_MAX, FLT_MAX, plotSize);

    ImGui::PopItemWidth();
    
    return result; 
}

float Tone::Evaluate(const Expression& InFunc, const ToneTiming& InTiming, const uint64 InFrame)
{
    TONE_SET(InFunc, FREQ, InTiming.GetFrequency(InFrame));
    TONE_SET(InFunc, NOTE, InTiming.GetNote(InFrame));
    TONE_SET(InFunc, MELODY, InTiming.GetMelody(InFrame));
    TONE_SET(InFunc, GLOBAL, InTiming.GetGlobal(InFrame));
    TONE_SET(InFunc, START, InTiming.GetStart(InFrame));
    TONE_SET(InFunc, END, InTiming.GetEnd(InFrame));
    return InFunc.Evaluate();
}

static float octaves(const float InValue, const float InOctaves)
{
    float val = 0.0f;
    for (int i = 0; i < static_cast<int>(InOctaves); i++)
        val += sinf(InValue / static_cast<float>(i + 1));
    return val / InOctaves;
}

static float smooth(const float InValue, const float InStart, const float InEnd)
{
    float val = Utility::Math::Saturate((InValue - InStart) / (InEnd - InStart));
    return val * val * (3.0f - 2.0f * val); 
}

void Tone::Declare(Expression& InFunc)
{
    TONE_DECLARE(InFunc, FREQ);
    TONE_DECLARE(InFunc, NOTE);
    TONE_DECLARE(InFunc, MELODY);
    TONE_DECLARE(InFunc, GLOBAL);
    TONE_DECLARE(InFunc, START);
    TONE_DECLARE(InFunc, END);
    InFunc.DeclareFunc("octaves", &octaves);
    InFunc.DeclareFunc("smooth", &smooth);
}
