#include "ToneLibrary.h"

bool ToneLibrary::Edit(const String &InName, uint32 InOffset)
{
    bool edited = false;
    auto& tones = Tones.Get();
    if (ImGui::Begin("Tone Library"))
    {
        int off = 0;
        int remove = -1;
        for (auto& tone : tones)
        {
            String offStr = std::to_string(off + 1);
            String name = offStr + "##Lib" + offStr;
            ImGui::SetNextItemWidth(-FLT_MIN);
            tone.Plot(tone.Name.Get());
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
                EditedTone = off;
            
            if (ImGui::BeginPopupContextItem(("Tone##Lib_Context" + offStr).c_str()))
            {
                if (ImGui::Selectable("Edit##Lib"))
                    EditedTone = off;
                if (ImGui::Selectable("Remove##Lib"))
                    remove = off;
                ImGui::EndPopup();
            }
            
            off++;
        }

        if (ImGui::Button("+##Lib"))
        {
            int nameI = 0;
            while (toneCache.contains("Unnamed " + std::to_string(nameI)))
                nameI++;
            EditedTone = static_cast<int>(tones.size());
            auto& t = tones.emplace_back();
            t.Name = "Unnamed " + std::to_string(nameI);
            edited = true;
        }
        
        if (remove >= 0)
        {
            tones.erase(tones.begin() + remove);
            edited = true;
            EditedTone = -1;
        }
    }
    ImGui::End();

    if (EditedTone != -1 && EditedTone < static_cast<int>(tones.size()))
    {
        auto& tone = tones[EditedTone];
        if (ImGui::Begin("Tone##Lib"))
        {
            if (ImGui::BeginPopupContextItem("Tone##Lib_Close"))
            {
                if (ImGui::Selectable("Close##Lib_Close"))
                    EditedTone = -1;
                ImGui::EndPopup();
            }
            edited |= tone.Edit();
        }
        ImGui::End();
    }

    if (edited)
        UpdateCache();
    
    return edited;
}

bool ToneLibrary::Contains(const String &InName)
{
    auto find = toneCache.find(InName);
    CHECK_RETURN(find == toneCache.end(), false);
    auto& tones = Tones.Get();
    CHECK_RETURN(find->second < 0 || find->second >= static_cast<int>(tones.size()), false);
    return true;
}

Tone& ToneLibrary::Get(const String &InName)
{
    int index = toneCache.at(InName);
    auto& tones = Tones.Get();
    CHECK_ASSERT(index < 0 || index >= static_cast<int>(tones.size()), "Index is out of range");
    return tones[index];
}

void ToneLibrary::UpdateCache()
{
    toneCache.clear();
    const auto& tones = Tones.Get();
    for (int i = 0; i < static_cast<int>(tones.size()); i++)
        toneCache[tones[i].Name] = i;
}
