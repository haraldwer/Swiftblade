#include "InfoPanelEditRoom.h"

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

void UI::InfoPanelEditRoom::Init(Container &InOwner)
{
    Container::Init(InOwner);

    Builder b = Builder(Container({
                .alignment = 1,
                .margins = 10
            }))
        .Push(List())
            .Push(Container({ .size = { 0, 200 }, .alignment = { 1, 0 }}))
                .Add(Image(), "Thumbnail")
                .Push(TabContainer({ .margins = 10, .anchor = {0, 1}, .pivot = {0, 1} }), "NameTab")
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

void UI::InfoPanelEditRoom::Update(Container &InOwner)
{
    Container::Update(InOwner);

    if (Get<ButtonDefault>("EditName").IsClicked())
        Get<TabContainer>("NameTab").Set("NameEdit");
    
    auto& tbx = Get<Textbox>("NameEdit");
    if (tbx.IsCommitted())
    {
        String text = tbx.GetText();
        if (auto res = room.Get())
        {
            if (text != res->data.Info.Get().Name.Get())
            {
                res->data.Info.Get().Name = text;
                res->Save();
            
                if (text.empty())
                    text = "Untitled";
                
                InstanceEvent<EditRoomEntryData>::Invoke({ room, false });
                
                // Apply new name!
                Get<Label>("Name").SetText(text);
            }
        }
        Get<TabContainer>("NameTab").Set("NameShow");
    }

    if (Get<ButtonDefault>("Open").IsClicked())
        if (auto editor = Engine::Manager::Get().Push<RoomEditor>())
            editor->SetRoom(room);
}

void UI::InfoPanelEditRoom::SetRoom(const ResEditRoom& InRoom)
{
    if (InRoom.Identifier().IsValid() && InRoom == room)
        return;
    room = InRoom;

    String thumb = InRoom.Identifier().Str() + ".png";
    Get<Image>("Thumbnail").SetProperties({ thumb });
    
    if (auto res = room.Get())
        SetResourceInfo(res->data);
}

void UI::InfoPanelEditRoom::SetText(const String &InEntry, const String &InText)
{
    Get<Label>(InEntry).SetText(InText);
}

void UI::InfoPanelEditRoom::SetResourceInfo(const EditRoom& InRoom)
{
    String name = InRoom.Info.Get().Name;
    if (name.empty())
        name = "Untitled";
    String creator = DB::Manager::Get().user.GetUsername();

    auto& req = InRoom.Info.Get();
    Vec3I size = req.Size;
    int objects = req.Objects;
    float length = req.Length;
    int spawners = req.Spawners;
    
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
