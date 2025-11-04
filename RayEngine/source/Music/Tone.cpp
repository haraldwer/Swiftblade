#include "Tone.h"

#ifdef IMGUI_ENABLE
#include "ImGui/imgui_custom.h"
#endif

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

bool Tone::Edit(const String &InName, uint32 InOffset)
{
    return PropertyOwner::Edit(InName, InOffset);
}

#ifdef IMGUI_ENABLE

bool Tone::Edit()
{
    bool result = false;
    
    ImGui::PushItemWidth(-FLT_MIN);
    constexpr ImVec2 plotSize = ImVec2(0, 50);
    
    Name.Edit();

    ToneTiming previewTiming;
    previewTiming.sampleRate = TONE_PLOT_ACCURACY;
    previewTiming.beatLength = TONE_PLOT_ACCURACY;
    previewTiming.noteLength = TONE_PLOT_ACCURACY / 4;

    ImGui::SeparatorText("Frequency");
    const bool freqEdit = Frequency.Edit();
    if (freqEdit || freqPreview.empty())
        UpdatePreview(Frequency, previewTiming, freqPreview);
    ImGui::PlotLines("##Freq", freqPreview.data(), static_cast<int>(freqPreview.size()), 0, 0, FLT_MAX, FLT_MAX, plotSize);
    result |= freqEdit;
    
    ImGui::SeparatorText("Volume");
    const bool volEdit = Volume.Edit();
    if (volEdit || volumePreview.empty())
        UpdatePreview(Volume, previewTiming, volumePreview);
    ImGui::PlotLines("##Vol", volumePreview.data(), static_cast<int>(volumePreview.size()), 0, 0, FLT_MAX, FLT_MAX, plotSize);
    result |= volEdit;

    if (result || tonePreview.empty())
        UpdateTonePreview(previewTiming);
    ImGui::SeparatorText("Tone");
    ImGui::PlotLines("##Tone", tonePreview.data(), static_cast<int>(tonePreview.size()), 0, 0, FLT_MAX, FLT_MAX, plotSize);

    ImGui::PopItemWidth();
    
    return result; 
}

void Tone::Plot(const String& InOverlay)
{
    if (tonePreview.empty())
    {
        ToneTiming previewTiming;
        previewTiming.sampleRate = TONE_PLOT_ACCURACY;
        previewTiming.beatLength = TONE_PLOT_ACCURACY;
        previewTiming.noteLength = TONE_PLOT_ACCURACY / 4;
        UpdateTonePreview(previewTiming);
    }
    
    constexpr ImVec2 plotSize = ImVec2(0, 50);
    ImGui::PlotLines("##Tone", tonePreview.data(), static_cast<int>(tonePreview.size()), 0, InOverlay.c_str(), FLT_MAX, FLT_MAX, plotSize);
}

#else

bool Tone::Edit() { return false; }
void Tone::Plot(const String &InOverlay) {  }

#endif

void Tone::UpdateTonePreview(const ToneTiming &InTiming)
{
    tonePreview.resize(TONE_PLOT_ACCURACY);
    for (uint64 i = 0; i < TONE_PLOT_ACCURACY; i++)
        tonePreview[i] = Utility::Math::Clamp(
            Evaluate(Frequency, InTiming, i) *
            Evaluate(Volume, InTiming, i),
            -1.0f,
            1.0f);
}

void Tone::UpdatePreview(const Expression& InExpr, const ToneTiming& InTiming, Vector<float>& InPreview)
{
    InPreview.resize(TONE_PLOT_ACCURACY);
    for (uint64 i = 0; i < TONE_PLOT_ACCURACY; i++)
        InPreview[i] = Evaluate(InExpr, InTiming, i);
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
    InFunc.Compile();
}
