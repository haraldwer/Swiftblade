#include "BrowseStoryPanel.h"

#include <ranges>

#include "Instance/InstanceManager.h"
#include "Instances/GameInstance.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"

void UI::BrowseStoryPanel::Init(Container &InOwner)
{
    BrowsePanel::Init(InOwner);
    
    config.LoadConfig();
    
    auto b = Builder()
        .Push(List());
    
    for (auto& e : std::ranges::reverse_view(config.Entries.Get()))
    {
        b.Add(Element({
                .size = { 5, 50},
                .anchor = 0.5,
                .pivot = 0.5,
            }, {
                .color = { 1 },
                .cornerRadius = 5
            }));
        b.Add(Label({
                .position = { 0, 0 },
                .size = { 100, 80},
                .anchor = 0.5,
                .pivot = 0.5,
            }, {
                e.Name,
                80,
                ResFont("UI/F_LinLibertine_aSZI.ttf")
            }), e.Name);
    }
    
    root = Add(b.Build());
}

void UI::BrowseStoryPanel::Update(Container &InOwner)
{
    BrowsePanel::Update(InOwner);
 
    for (auto& e : std::ranges::reverse_view(config.Entries.Get()))
        if (Get<Element>(e.Name).IsClicked())
            if (auto game = Engine::InstanceManager::Get().Push<GameInstance>())
                game->PlayScene({ ResScene("Cache/test.json") });
            
}

bool UI::BrowseStoryPanel::DebugDraw(Container &InOwner, const String &InIdentifier, int &InC)
{
    if (GetVisible())
    {
        if (ImGui::Begin(("Story##" + Utility::ToStr(InC)).c_str()))
            if (config.Edit())
                config.SaveConfig();
        ImGui::End();
    }
    
    return BrowsePanel::DebugDraw(InOwner, InIdentifier, InC);
}
