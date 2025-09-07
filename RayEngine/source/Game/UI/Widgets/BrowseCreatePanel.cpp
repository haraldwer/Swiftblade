#include "BrowseCreatePanel.h"

#include "TabButton.h"
#include "UI/Builder.h"
#include "UI/Elements/List.h"
#include "UI/Elements/TabContainer.h"

void UI::BrowseCreatePanel::Init(Container &InOwner)
{
    BrowsePanel::Init(InOwner);

    auto b = Builder()
        .Push(List(Transform::Fill({ 10 })))
            .Push(List({}, 0, 0, List::FlowDirection::HORIZONTAL))
                .Add(TabButton("Levels"), "Levels")
                .Add(TabButton("Rooms"), "Rooms")
            .Pop()
            .Push(TabContainer(), "Tabs")
                .Add(List(), "RoomList")
                .Add(List(), "LevelList");
    
    root = Add(b.Build());

    SetBackground({
        .color = { 1, 1, 1, 0.5 }
    });

    SelectLevels(); 
}

void UI::BrowseCreatePanel::Update(Container &InOwner)
{
    BrowsePanel::Update(InOwner);

    if (Get<TabButton>("Levels").IsClicked())
        SelectLevels();
    if (Get<TabButton>("Rooms").IsClicked())
        SelectRooms();
}

void UI::BrowseCreatePanel::SelectLevels()
{
    Get<TabButton>("Levels").SetSelected(true);
    Get<TabButton>("Rooms").SetSelected(false);
    Get<TabContainer>("Tabs").Set("LevelList");

    Vector<String> files = Utility::ListFiles("User/Levels");
    for (auto& f : files)
        LOG("Found level: " + Utility::ReadFile(f));

    auto& list = Get<List>("LevelList");
    list.ClearChildren();
}

void UI::BrowseCreatePanel::SelectRooms()
{
    Get<TabButton>("Levels").SetSelected(false);
    Get<TabButton>("Rooms").SetSelected(true);
    Get<TabContainer>("Tabs").Set("RoomList");

    Vector<String> files = Utility::ListFiles("User/Rooms");
    for (auto& f : files)
        LOG("Found room: " + Utility::ReadFile(f));
    
}
