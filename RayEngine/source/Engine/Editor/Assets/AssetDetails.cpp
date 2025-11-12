#include "AssetDetails.h"

#include "BlueprintResourceDetails.h"
#include "Config.h"
#include "TextDetails.h"
#include "ResourceDetails.h"
#include "Blueprints/Blueprint.h"
#include "Physics/Resources/Material.h"
#include "Resources/Material.h"
#include "Resources/Model.h"
#include "Resources/NoiseTexture.h"
#include "UI/Resources/Font.h"
#include "ConfigDetails.h"
#include "Debug/Config.h"
#include "Editor/BlueprintEditor.h"
#include "Input/Config.h"
#include "Instance/Launcher.h"
#include "Resources/Particle.h"
#include "UI/Widgets/Browse/BrowseStoryPanel.h"

void AssetDetails::DrawDebugPanel()
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
}

void AssetDetails::Clear()
{
    for (auto& detail : details)
        detail.Get().Deinit();
    details.clear();
}

void AssetDetails::Select(const Vector<String> &InPaths)
{
    for (auto& path : InPaths)
        OpenPanel(path);
}

void AssetDetails::Deselect(const String& InPath)
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

void AssetDetails::OpenPanel(const String& InPath)
{
    LOG("Details: " + InPath);
    
#define ASSET_OPEN(T) { details.push_back(T(InPath)); details.at(details.size() - 1).Get().Init(); return; }
#define ASSET_TRY_OPEN(T) if (T::Accept(InPath)) ASSET_OPEN(T)
#define ASSET_TRY_OPEN_FUNC(T, acceptFunc, acceptParam) if (acceptFunc(acceptParam, InPath)) ASSET_OPEN(T)

    ASSET_TRY_OPEN(FileResourceDetails<Rendering::TextureResource>);
    ASSET_TRY_OPEN(FileResourceDetails<Rendering::ModelResource>);
    ASSET_TRY_OPEN(FileResourceDetails<UI::FontResource>);
    
    ASSET_TRY_OPEN(TextResourceDetails<Rendering::NoiseTextureResource>);
    ASSET_TRY_OPEN(TextResourceDetails<Rendering::BakedTexture>);
    ASSET_TRY_OPEN(TextResourceDetails<Rendering::Particle>);
    ASSET_TRY_OPEN(BlueprintResourceDetails);
    
    ASSET_TRY_OPEN(ConfigDetails<Rendering::Config>);
    ASSET_TRY_OPEN(ConfigDetails<Input::Config>);
    ASSET_TRY_OPEN(ConfigDetails<Engine::LauncherConfig>);
    ASSET_TRY_OPEN(ConfigDetails<BlueprintEditorConfig>);
    ASSET_TRY_OPEN(ConfigDetails<Debug::Config>);

    auto jsonPrefixFunc = [](const String& InPrefix, const String& InPath)
    {
        return Utility::File::Name(InPath).starts_with(InPrefix) && InPath.ends_with(".json");
    };
    
    ASSET_TRY_OPEN_FUNC(TextResourceDetails<PhysicsMaterialResource>, jsonPrefixFunc, "PM_");
    ASSET_TRY_OPEN_FUNC(TextResourceDetails<Rendering::MaterialResource>, jsonPrefixFunc, "RM_");
    
    ASSET_TRY_OPEN(TextDetails);
    ASSET_TRY_OPEN(AssetDetailPanel);

#undef ASSET_OPEN
#undef ASSET_TRY_OPEN
#undef ASSET_TRY_OPEN_FUNC
}
