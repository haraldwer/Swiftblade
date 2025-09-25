#include "InfoPanelRoom.h"

#include "Database/Manager.h"
#include "Database/Data/RPCLevelInfo.h"
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
#include "UI/Widgets/RoomList/RoomEntryWidget.h"

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
        if (auto editor = Engine::Manager::Get().Push<RoomEditor>())
            editor->SetRoom(data.resource);
}

void UI::InfoPanelRoom::SetRoom(const RoomEntryData &InData)
{
    if (!data.entry.ID.Get().empty() && data.entry.ID == InData.entry.ID)
        return;
    if (data.resource.Identifier().IsValid() && data.resource == InData.resource)
        return;
    
    data = InData;

    String name = InData.entry.Name;
    String creator = InData.entry.Creator;
    Vec3I size = Vec3I::Zero();
    int objects = 0;
    float length = 0.0f;
    int spawners = 0;
    
    if (!data.resource.Identifier().IsValid() && !data.entry.ID.Get().empty())
        data.resource = Utility::GetCachePath(data.entry.ID, ".json");
    
    if (auto res = data.resource.Get())
    {
        name = res->data.Name.Get();

        // Use data from last request
        auto& req = res->data.LastRequest.Get();
        size = req.Size;
        objects = req.Objects;
        length = req.Length;
        spawners = req.Spawners;
    }
    else
    {
        // Request from server
        DB::RPCRoomInfo::Request request;
        request.ID = data.entry.ID;
        DB::Manager::Get().rpc.Request<DB::RPCRoomInfo>(request);

        // Wait for response?
    }

    if (name.empty())
        name = "Untitled";
    if (creator.empty())
        creator = "you";
    
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

void UI::InfoPanelRoom::SetText(const String &InEntry, const String &InText)
{
    Get<Label>(InEntry).SetText(InText);
}
