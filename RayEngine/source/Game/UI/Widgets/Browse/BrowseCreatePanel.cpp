#include "BrowseCreatePanel.h"

#include "GUID.h"
#include "../LevelList/LevelListWidget.h"
#include "../EditRoomList/EditRoomEntryWidget.h"
#include "../Common/ButtonTab.h"
#include "Editor/EditRoom.h"
#include "UI/Builder.h"
#include "UI/Elements/List.h"
#include "UI/Elements/TabContainer.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/LevelList/LevelEntryWidget.h"

void UI::BrowseCreatePanel::Init(Container &InOwner)
{
    BrowsePanel::Init(InOwner);

    auto b = Builder()
        .Push(List(Transform::Fill({ 10 })))
            .Push(List({}, { 10, ListDirection::HORIZONTAL }))
                .Add(ButtonTab("Levels"), "Levels")
                .Add(ButtonTab("Rooms"), "Rooms")
            .Pop()
            .Push(TabContainer(Transform::Fill({{}, { 5 }}), {}, true), "Tabs")
                .Add(List(Transform::Fill(), { .scrollable = true }), "RoomList")
                .Add(LevelListWidget(), "LevelList");    
    
    root = Add(b.Build());

    SetBackground({
        .color = { 1, 1, 1, 0.5 }
    });

    onAddedLevel.Bind([](const auto& InData, auto InC)
    {
        InC->NewLevel(InData);
    });

    onAddedRoom.Bind([](const auto& InData, auto InC)
    {
        InC->NewRoom(InData);
    });

    onInstanceRemoved.Bind([](const auto& InData, auto InC)
    {
        InC->pendingRefresh = true;
    });

    pendingRefresh = true;
}

void UI::BrowseCreatePanel::Update(Container &InOwner)
{
    if (pendingRefresh)
    {
        pendingRefresh = false;
        levelsSelected ? SelectLevels() : SelectRooms();
    }
    
    BrowsePanel::Update(InOwner);

    if (Get<ButtonTab>("Levels").IsClicked())
        SelectLevels();
    if (Get<ButtonTab>("Rooms").IsClicked())
        SelectRooms();
}

void UI::BrowseCreatePanel::NewLevel(const LevelEntryData &InData)
{
    CHECK_RETURN(!InData.add);
    
    // Create new level file
    String hash = Utility::NewGUID();
    CHECK_RETURN_LOG(hash.empty(), "Failed to generate hash");
    String fileName = "L_" + hash + ".json";
    String path = "User/Levels/" + fileName;
    CHECK_RETURN_LOG(Utility::FileExists(path), "Failed to create file, it already exists");
    LOG("Creating level file: " + path);
    Utility::CreateDir(path);
    Utility::WriteFile(path, "{}");
    if (!Utility::FileExists(path))
        LOG("Failed to create level: " + path);

    LevelEntryData newData;
    newData.resource = path;

    auto& list = Get<LevelListWidget>("LevelList");
    LevelEntryWidget w(newData);
    w.Init(list);
    list.Insert(w, list.Count() - 1);
    
    InstanceEvent<LevelEntryData>::Invoke(newData); // Select the new entry
}

void UI::BrowseCreatePanel::NewRoom(const EditRoomEntryData& InData)
{
    CHECK_RETURN(!InData.add);
    
    // Create new room file
    String hash = Utility::NewGUID();
    CHECK_RETURN_LOG(hash.empty(), "Failed to generate hash");
    String fileName = "R_" + hash + ".json";
    String path = "User/Rooms/" + fileName;
    CHECK_RETURN_LOG(Utility::FileExists(path), "Failed to create file, it already exists");
    LOG("Creating room file: " + path);
    Utility::CreateDir(path);
    Utility::WriteFile(path, "{}");
    if (!Utility::FileExists(path))
        LOG("Failed to create room: " + path);

    EditRoomEntryData newData;
    newData.resource = path;

    auto& list = Get<List>("RoomList");
    EditRoomEntryWidget w(newData);
    w.Init(list);
    list.Insert(w, list.Count() - 1);
    
    InstanceEvent<EditRoomEntryData>::Invoke(newData);
}

void UI::BrowseCreatePanel::SelectLevels()
{
    levelsSelected = true;
    Get<ButtonTab>("Levels").SetSelected(true);
    Get<ButtonTab>("Rooms").SetSelected(false);
    Get<TabContainer>("Tabs").Set("LevelList");

    auto& list = Get<LevelListWidget>("LevelList");
    list.ClearChildren();
    
    for (auto& f : Utility::ListFiles("User/Levels"))
    {
        LevelEntryData data;
        data.resource = f;
        auto w = LevelEntryWidget(data);
        w.Init(list);
        list.Add(w);
    }

    LevelEntryData addData;
    addData.add = true;
    auto add = LevelEntryWidget(addData);
    add.Init(list);
    list.Add(add);
}

void UI::BrowseCreatePanel::SelectRooms()
{
    levelsSelected = false;
    Get<ButtonTab>("Levels").SetSelected(false);
    Get<ButtonTab>("Rooms").SetSelected(true);
    Get<TabContainer>("Tabs").Set("RoomList");
    
    auto& list = Get<List>("RoomList");
    list.ClearChildren();
    
    for (auto& f : Utility::ListFiles("User/Rooms"))
    {
        if (f.ends_with(".json"))
        {
            EditRoomEntryData data;
            data.resource = f;
            auto w = EditRoomEntryWidget(data);
            w.Init(list);
            list.Add(w);
        }
    }

    EditRoomEntryData addData;
    addData.add = true;
    auto add = EditRoomEntryWidget(addData);
    add.Init(list);
    list.Add(add);
}
