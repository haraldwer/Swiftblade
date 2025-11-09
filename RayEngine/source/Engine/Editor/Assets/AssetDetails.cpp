#include "AssetDetails.h"

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
        
        String name = "Details: " + Utility::Filename(details[i].Get().path);
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
#define ASSET_OPEN(T) { details.push_back(T(InPath)); details.at(details.size() - 1).Get().Init(); return; }
#define ASSET_TRY_OPEN(T) if (T::Accept(InPath)) ASSET_OPEN(T)
#define ASSET_TRY_OPEN_FUNC(T, acceptFunc, acceptParam) if (acceptFunc(acceptParam, InPath)) ASSET_OPEN(T)

    ASSET_TRY_OPEN(ResourceDetails<Rendering::TextureResource>);
    ASSET_TRY_OPEN(ResourceDetails<Rendering::NoiseTextureResource>);
    ASSET_TRY_OPEN(ResourceDetails<Rendering::ModelResource>);
    ASSET_TRY_OPEN(ResourceDetails<Rendering::BakedTexture>);
    ASSET_TRY_OPEN(ResourceDetails<Rendering::Particle>);
    ASSET_TRY_OPEN(ResourceDetails<UI::FontResource>);
    ASSET_TRY_OPEN(ResourceDetails<BlueprintResource>);
    
    ASSET_TRY_OPEN(ConfigDetails<Rendering::Config>);
    ASSET_TRY_OPEN(ConfigDetails<Input::Config>);
    ASSET_TRY_OPEN(ConfigDetails<Engine::LauncherConfig>);
    ASSET_TRY_OPEN(ConfigDetails<StoryConfig>);

    auto jsonPrefixFunc = [](const String& InPrefix, const String& InPath)
    {
        return Utility::Filename(InPath).starts_with(InPrefix) && InPath.ends_with(".json");
    };
    
    ASSET_TRY_OPEN_FUNC(ResourceDetails<PhysicsMaterialResource>, jsonPrefixFunc, "PM_");
    ASSET_TRY_OPEN_FUNC(ResourceDetails<Rendering::MaterialResource>, jsonPrefixFunc, "RM_");
    
    ASSET_TRY_OPEN(TextDetails);
    ASSET_TRY_OPEN(AssetDetailPanel);

#undef ASSET_OPEN
#undef ASSET_TRY_OPEN
#undef ASSET_TRY_OPEN_FUNC
}
