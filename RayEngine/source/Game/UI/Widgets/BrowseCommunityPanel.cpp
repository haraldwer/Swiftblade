#include "BrowseCommunityPanel.h"

#include "LevelEntryWidget.h"
#include "Separator.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"

void UI::BrowseCommunityPanel::Init(Container &InOwner)
{
    BrowsePanel::Init(InOwner);
    
    auto b = Builder()
        .Push(List(Transform::Fill({ 10 })))
            .Add(Label({}, "Submissions"))
            .Add(Separator())
            .Push(List(Transform::Fill()))
                .Add(LevelEntryWidget("Entry"), "Entry");
            //.Add(Label({}, "Challenges"))
            //.Add(Label({}, "Daily"))
            //.Add(List(Transform::Fill()))
            //.Add(Label({}, "Weekly"))
            //.Add(List(Transform::Fill()))
            //.Add(Label({}, "Monthly"))
            //.Push(List(Transform::Fill()))

    entries.push_back("Entry");
    root = Add(b.Build());
    
    SetBackground({
        .color = { 1, 1, 1, 0.5 }
    });
}

void UI::BrowseCommunityPanel::Update(Container &InOwner)
{
    BrowsePanel::Update(InOwner);

    auto& r = Get<Container>(root);
    for (auto& e : entries)
        if (r.Get<LevelEntryWidget>(e).IsClicked())
            levelSelectedEvent.Invoke({ e });
}
