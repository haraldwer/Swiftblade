#include "BrowseCreatePanel.h"

#include "../LevelList/LevelListWidget.h"
#include "../RoomEntryWidget.h"
#include "../Common/ButtonTab.h"
#include "Editor/Room/RoomResource.h"
#include "UI/Builder.h"
#include "UI/Elements/List.h"
#include "UI/Elements/TabContainer.h"

void UI::BrowseCreatePanel::Init(Container &InOwner)
{
    BrowsePanel::Init(InOwner);

    auto b = Builder()
        .Push(List(Transform::Fill({ 10 })))
            .Push(List({}, { 10, ListDirection::HORIZONTAL }))
                .Add(ButtonTab("Levels"), "Levels")
                .Add(ButtonTab("Rooms"), "Rooms")
            .Pop()
            .Push(TabContainer(Transform::Fill({{}, { 5, 0 }})), "Tabs")
                .Add(List(), "RoomList")
                .Add(LevelListWidget(), "LevelList");
    
    root = Add(b.Build());

    SetBackground({
        .color = { 1, 1, 1, 0.5 }
    });

    SelectLevels(); 
}

void UI::BrowseCreatePanel::Update(Container &InOwner)
{
    BrowsePanel::Update(InOwner);

    if (Get<ButtonTab>("Levels").IsClicked())
        SelectLevels();
    if (Get<ButtonTab>("Rooms").IsClicked())
        SelectRooms();
}

void UI::BrowseCreatePanel::SelectLevels()
{
    Get<ButtonTab>("Levels").SetSelected(true);
    Get<ButtonTab>("Rooms").SetSelected(false);
    Get<TabContainer>("Tabs").Set("LevelList");

    Vector<String> files = Utility::ListFiles("User/Levels");
    for (auto& f : files)
        LOG("Found level: " + Utility::ReadFile(f));

    auto& list = Get<LevelListWidget>("LevelList");
    list.ClearChildren();
}

void UI::BrowseCreatePanel::SelectRooms()
{
    Get<ButtonTab>("Levels").SetSelected(false);
    Get<ButtonTab>("Rooms").SetSelected(true);
    Get<TabContainer>("Tabs").Set("RoomList");
    
    auto& list = Get<List>("RoomList");
    list.ClearChildren();
    
    Vector<String> files = Utility::ListFiles("User/Rooms");
    for (auto& f : files)
    {
        auto w = RoomEntryWidget(f);
        w.Init(list);
        list.Add(w);
    }

    auto add = RoomEntryWidget();
    add.Init(list);
    list.Add(add);
}
