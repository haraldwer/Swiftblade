#include "BrowseCommunityPanel.h"

#include "LevelEntryWidget.h"
#include "LevelListWidget.h"
#include "Separator.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"

void UI::BrowseCommunityPanel::Init(Container &InOwner)
{
    BrowsePanel::Init(InOwner);
    
    auto b = Builder()
        .Push(List(Transform::Fill({ 10 })))
            .Add(LevelListWidget("Submissions"))
            .Add(LevelListWidget("Daily"))
            .Add(LevelListWidget("Weekly"))
            .Add(LevelListWidget("Monthly"));
    
    root = Add(b.Build());
    
    SetBackground({
        .color = { 1, 1, 1, 0.5 }
    });
}

void UI::BrowseCommunityPanel::Update(Container &InOwner)
{
    BrowsePanel::Update(InOwner);

    String level;
    String listing;
    for (auto e : children)
    {
        if (auto w = TryGet<LevelListWidget>(e))
        {
            String s = w->GetSelected();
            if (!s.empty())
            {
                level = s;
                listing = w->GetListing();
            }
        }
    }

    if (!level.empty())
        levelSelectedEvent.Invoke({ level, listing });
}
