#include "InfoPanelLevel.h"

#include "Database/Manager.h"
#include "Database/Data/RPCLevel.h"
#include "Instance/Manager.h"
#include "Instances/GameInstance.h"
#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/List.h"
#include "UI/Elements/TabContainer.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"
#include "UI/Widgets/Common/NumberSelector.h"
#include "UI/Widgets/Common/TextboxDefault.h"
#include "UI/Widgets/LevelList/LevelEntryWidget.h"
#include "UI/Widgets/LevelRoomList/LevelRoomList.h"

void UI::InfoPanelLevel::Init(Container &InOwner)
{
    Container::Init(InOwner);

    Builder b = Builder(Container({
                .alignment = 1,
                .margins = 10
            }))
        .Push(List())
            .Push(TabContainer(), "NameTab")
                .Push(List({}, { 10, ListDirection::HORIZONTAL}), "NameShow")
                    .Add(ButtonDefault({ .anchor = 0.5, .pivot = 0.5 }, " "), "EditName")
                    .Push(List({ .anchor = { 0, 0.5 }, .pivot = { 0, 0.5 }}))
                        .Add(LabelHeader(), "Name")
                        .Add(LabelText({.padding = {0, {-5, 0}}}), "Creator")
                    .Pop()
                .Pop()
                .Add(TextboxDefault({
                        .alignment = { 1, 1 }
                    }, {
                        .maxChars = 25,
                        .centering = { 0, 0.5 },
                    }), "NameEdit")
            .Pop()
            .Add(NumberSelector(Transform::Fill(), { "Number of rooms", 5, 1, 1, {2, 10} }), "NumRooms")
            .Add(LabelHeader(), "Rooms")
            .Push(LevelRoomList(Transform::Fill(10)), "RoomList")
                // Add rooms here    
            .Pop()
            .Add(NumberSelector(Transform::Fill(), { "Number of arenas", 3, 1, 1, {1, 5}}), "NumArenas")
            .Add(LabelHeader(), "Arenas")
            .Push(LevelRoomList(Transform::Fill(10)), "ArenaList")
                // Add arenas here
            .Pop()
        .Pop()
        .Push(List({
                .position = {},
                .anchor = {1, 1},
                .pivot = {1, 1}
            },  {
                .direction = ListDirection::HORIZONTAL
            }))
            .Add(ButtonDefault({}, "Play"), "Play");
    Add(b.Build());

    Get<TabContainer>("NameTab").Set("NameShow");

    onInfo.Bind([](const auto& InData, auto InC)
    {
        InC->RecieveInfo(InData);
    });
}

void UI::InfoPanelLevel::Update(Container &InOwner)
{
    Container::Update(InOwner);

    if (Get<ButtonDefault>("EditName").IsClicked())
        Get<TabContainer>("NameTab").Set("NameEdit");

    auto& tbx = Get<Textbox>("NameEdit");
    if (tbx.IsCommitted())
    {
        String text = tbx.GetText();
        if (text != data.entry.Name.Get())
        {
            if (text.empty())
                text = "Untitled";

            if (auto res = data.resource.Get())
            {
                res->data.Name = text;
                res->Save();
            }

            data.entry.Name = text;
            InstanceEvent<LevelEntryData>::Invoke(data);
            
            // Apply new name!
            Get<Label>("Name").SetText(text);
        }
        
        Get<TabContainer>("NameTab").Set("NameShow");
    }
    
    if (Get<ButtonDefault>("Play").IsClicked())
    {
        if (auto game = Engine::Manager::Get().Push<GameInstance>())
        {
            LevelConfig c;
            game->PlayLevel(c);
        }
    }
}

void UI::InfoPanelLevel::SetLevel(const LevelEntryData &InData)
{
    if (!data.entry.ID.Get().empty() && data.entry.ID == InData.entry.ID)
        return;
    if (data.resource.Identifier().IsValid() && data.resource == InData.resource)
        return;
    
    data = InData;
    SetEntryInfo(data.entry);
    
    // Not a local file, check cache
    if (!data.resource.Identifier().IsValid() && !data.entry.ID.Get().empty())
        data.resource = Utility::GetCachePath(data.entry.ID, ".json");
    
    if (auto res = data.resource.Get())
    {
        SetResourceInfo(res->data);
    }
    else
    {
        // Request from server
        DB::RPCLevelInfo::Request request;
        request.ID = data.entry.ID;
        DB::Manager::Get().rpc.Request<DB::RPCLevelInfo>(request);

        // Show loading
    }
}

void UI::InfoPanelLevel::RecieveInfo(const DB::Response<DB::RPCLevelInfo> &InResponse)
{
    if (!InResponse.success)
    {
        LOG("RPC failed")
        return;
    }

    if (InResponse.data.ID != data.entry.ID)
    {
        LOG("ID mismatch");
        return;
    }
    
    LOG("Received info!")
    
    // Create cache level
    String path = Utility::GetCachePath(InResponse.data.ID, ".json");
    if (!InResponse.data.Data.Get().Save(path))
    {
        LOG("Failed to save: " + path)
        return;
    }

    // Set the cache path
    data.resource = path;

    // Update info
    if (auto res = data.resource.Get())
        SetResourceInfo(res->data);
}

void UI::InfoPanelLevel::SetEntryInfo(DB::RPCLevelList::Entry InEntry)
{
    String name = InEntry.Name;
    String creator = InEntry.Creator;

    if (name.empty())
        name = "Untitled";
    if (creator.empty())
        creator = "you";
    Get<Label>("Name").SetText(name);
    Get<Textbox>("NameEdit").SetText(name);
    Get<Label>("Creator").SetText("by " + creator);
}

void UI::InfoPanelLevel::SetResourceInfo(const Level& InData)
{
    String name = InData.Name;
    String creator = InData.Creator;

    if (name.empty())
        name = "Untitled";
    if (creator.empty())
        creator = "you";
    Get<Label>("Name").SetText(name);
    Get<Textbox>("NameEdit").SetText(name);
    Get<Label>("Creator").SetText("by " + creator);
    
    Get<NumberSelector>("NumRooms").SetValue(InData.NumRooms);
    Get<LevelRoomList>("RoomList").SetEntries(InData.Rooms);
    Get<NumberSelector>("NumArenas").SetValue(InData.NumArenas);
    Get<LevelRoomList>("ArenaList").SetEntries(InData.Arenas);
}