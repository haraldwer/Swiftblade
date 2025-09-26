#include "InfoPanelRoom.h"

#include "Database/Manager.h"
#include "Database/Data/RPCLevel.h"
#include "Editor/RoomEditor.h"
#include "Instance/Manager.h"
#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"
#include "UI/Elements/TabContainer.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"
#include "UI/Widgets/Common/TextboxDefault.h"
#include "UI/Widgets/EditRoomList/EditRoomEntryWidget.h"

void UI::InfoPanelRoom::Init(Container &InOwner)
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
            .Add(LabelText(), "Length")
            .Add(LabelText(), "Size")
            .Add(LabelText(), "Objects")
            .Add(LabelText(), "Enemies")
            .Add(LabelText(), "Status")
            .Add(LabelText(), "Date")
        .Pop()
        .Push(List({
                .position = {},
                .anchor = {1, 1},
                .pivot = {1, 1}
            },  {
                .direction = ListDirection::HORIZONTAL
            }))
            .Add(ButtonDefault({}, "Open"), "Open");
    Add(b.Build());

    Get<TabContainer>("NameTab").Set("NameShow");

    onInfo.Bind([](const auto& InData, auto InC)
    {
        InC->RecieveInfo(InData);
    });
}

void UI::InfoPanelRoom::Update(Container &InOwner)
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
            InstanceEvent<RoomEntryData>::Invoke(data);
            
            // Apply new name!
            Get<Label>("Name").SetText(text);
        }
        Get<TabContainer>("NameTab").Set("NameShow");
    }

    if (Get<ButtonDefault>("Open").IsClicked())
    {
        
    }
}

void UI::InfoPanelRoom::SetRoom(const RoomEntryData &InData)
{
    if (!data.entry.ID.Get().empty() && data.entry.ID == InData.entry.ID)
        return;
    if (data.resource.Identifier().IsValid() && data.resource == InData.resource)
        return;
    
    data = InData;
    SetEntryInfo(data.entry);

    if (!data.resource.Identifier().IsValid() && !data.entry.ID.Get().empty())
        data.resource = Utility::GetCachePath(data.entry.ID, ".json");
    
    if (auto res = data.resource.Get())
    {
        SetResourceInfo(res->data);
    }
    else
    {
        // Request from server
        DB::RPCRoomInfo::Request request;
        request.ID = data.entry.ID;
        DB::Manager::Get().rpc.Request<DB::RPCRoomInfo>(request);

        // Show loading
    }
}

void UI::InfoPanelRoom::SetText(const String &InEntry, const String &InText)
{
    Get<Label>(InEntry).SetText(InText);
}

void UI::InfoPanelRoom::SetEntryInfo(const RoomEntry &InEntry)
{
    String name = InEntry.Name;
    String creator = InEntry.Creator;
    
    if (name.empty())
        name = "Untitled";
    if (creator.empty())
        creator = "you";
    
    SetText("Name", name);
    Get<Textbox>("NameEdit").SetText(name);
    SetText("Creator", "by " + creator);

    // Hide these? 
    SetText("Length", "Length: ");
    SetText("Size", "Size: ");
    SetText("Objects", "Objects: ");
    SetText("Enemies", "Spawners: ");
    SetText("Status", "Status: ");
    SetText("Date", "Date: ");
}

void UI::InfoPanelRoom::SetResourceInfo(const Room &InRoom)
{
    String name = InRoom.Name;
    String creator = InRoom.Creator;

    if (name.empty())
        name = "Untitled";
    if (creator.empty())
        creator = "you";
    
    Vec3I size = InRoom.Size;
    int objects = InRoom.Objects;
    float length = InRoom.Length;
    int spawners = InRoom.Spawners;
    
    SetText("Name", name);
    Get<Textbox>("NameEdit").SetText(name);
    SetText("Creator", "by " + creator);
    SetText("Length", "Length: " + Utility::ToStr(length) + " units");
    SetText("Size", "Size: " + Utility::ToStr(size.x) + "x" + Utility::ToStr(size.y) + "x" + Utility::ToStr(size.z));
    SetText("Objects", "Objects: " + Utility::ToStr(objects));
    SetText("Enemies", "Spawners: " + Utility::ToStr(spawners));
    SetText("Status", "Status: -");
    SetText("Date", "Date: -");
}

void UI::InfoPanelRoom::RecieveInfo(const DB::Response<DB::RPCRoomInfo>& InResponse)
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
