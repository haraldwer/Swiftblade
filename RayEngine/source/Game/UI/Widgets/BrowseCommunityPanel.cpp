#include "BrowseCommunityPanel.h"

#include "LevelEntryWidget.h"
#include "LevelListWidget.h"
#include "Separator.h"
#include "UI/Builder.h"
#include "UI/Elements/List.h"

void UI::BrowseCommunityPanel::Init(Container &InOwner)
{
    BrowsePanel::Init(InOwner);
    
    auto b = Builder()
        .Push(List(Transform::Fill({ 10 })))
            .Add(LevelListWidget("submissions", "Submissions"))
            .Add(LevelListWidget("", "All-time"))
            .Add(LevelListWidget("weekly", "Weekly"))
            .Add(LevelListWidget("monthly", "Monthly"));
    
    root = Add(b.Build());
    
    SetBackground({
        .color = { 1, 1, 1, 0.5 }
    });
}
