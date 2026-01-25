#include "AssetDetails.h"

#include "ECS/BlueprintResourceDetails.h"
#include "TextDetails.h"
#include "ResourceDetails.h"
#include "Physics/Resources/Material.h"
#include "Resources/Material.h"
#include "Resources/Model.h"
#include "Resources/NoiseTexture.h"
#include "UI/Resources/Font.h"
#include "ConfigDetails.h"
#include "Debug/Config.h"
#include "ECS/BlueprintEditor.h"
#include "Input/Config.h"
#include "Launcher/Launcher.h"
#include "Resources/Particle.h"

void Editor::AssetDetails::DrawPanel()
{
    int counter = 0;
    for (int i = 0; i < (int)details.size(); i++)
    {
        auto& detail = details[i];
        bool open = true;
        
        String name = "Details: " + Utility::File::Name(details[i].Get().path);
        if (counter != 0)
            ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(10000, 10000));
        if (counter == 0 || ImGui::Begin(name.c_str(), &open))
            detail.Get().Draw();
        if (counter != 0)
            ImGui::End();
        counter++;

        if (!open)
        {
            detail.Get().Deinit();
            details.erase(details.begin() + i);
            i--;
        }
    }

    if (counter == 0)
    {
        auto size = ImGui::CalcTextSize("No asset selected");
        auto win = ImGui::GetWindowSize();
        ImGui::SetCursorPos((win - size) / 2);
        ImGui::Text("No asset selected");
    }
}

void Editor::AssetDetails::Clear()
{
    for (auto& detail : details)
        detail.Get().Deinit();
    details.clear();
}

void Editor::AssetDetails::Select(const Vector<String> &InPaths)
{
    for (auto& path : InPaths)
        OpenPanel(path);
}

void Editor::AssetDetails::Deselect(const String& InPath)
{
    for (int i = static_cast<int>(details.size()) - 1; i >= 0; i--)
    {
        if (details[i].Get().path == InPath)
        {
            details[i].Get().Deinit();
            details.erase(details.begin() + i);
        }
    }
}

void Editor::AssetDetails::OpenPanel(const String& InPath)
{
    LOG("Details: " + InPath);
    
#define ASSET_OPEN(T) { details.push_back(T(InPath)); details.at(details.size() - 1).Get().Init(); return; }
#define ASSET_TRY_OPEN(T) if (T::Accept(InPath)) ASSET_OPEN(T)

    ASSET_TRY_OPEN(FileResourceDetails<Rendering::TextureResource>);
    ASSET_TRY_OPEN(FileResourceDetails<Rendering::ModelResource>);
    ASSET_TRY_OPEN(FileResourceDetails<UI::FontResource>);
    
    ASSET_TRY_OPEN(TextResourceDetails<Rendering::NoiseTextureResource>);
    //ASSET_TRY_OPEN(TextResourceDetails<Rendering::BakedTexture>);
    ASSET_TRY_OPEN(TextResourceDetails<Rendering::Particle>);
    ASSET_TRY_OPEN(TextResourceDetails<PhysicsMaterialResource>);
    ASSET_TRY_OPEN(TextResourceDetails<Rendering::MaterialResource>);
    ASSET_TRY_OPEN(BlueprintResourceDetails);
    
    //ASSET_TRY_OPEN(ConfigDetails<Rendering::Config>);
    ASSET_TRY_OPEN(ConfigDetails<Input::Config>);
    ASSET_TRY_OPEN(ConfigDetails<LauncherConfig>);
    ASSET_TRY_OPEN(ConfigDetails<BlueprintEditorConfig>);
    ASSET_TRY_OPEN(ConfigDetails<Debug::Config>);
    
    ASSET_TRY_OPEN(TextDetails);
    ASSET_TRY_OPEN(AssetDetailPanel);

#undef ASSET_OPEN
#undef ASSET_TRY_OPEN
}
