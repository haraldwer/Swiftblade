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
}

void UI::InfoPanelRoom::Update(Container &InOwner)
{
    Container::Update(InOwner);

    auto res = resource.Get();
    CHECK_RETURN(!res);
    auto& info = res->data;
    
    if (Get<ButtonDefault>("EditName").IsClicked())
        Get<TabContainer>("NameTab").Set("NameEdit");
    
    auto& tbx = Get<Textbox>("NameEdit");
    if (tbx.IsCommitted())
    {
        String text = tbx.GetText();
        if (text != info.Name.Get())
        {
            if (text.empty())
                text = "Untitled";

            res->Save();
            InstanceEvent<RoomInfo>::Invoke(info);
            
            // Apply new name!
            Get<Label>("Name").SetText(text);
        }
        Get<TabContainer>("NameTab").Set("NameShow");
    }

    if (Get<ButtonDefault>("Open").IsClicked())
    {
        
    }
}

void UI::InfoPanelRoom::SetRoom(const ResRoom &InResource)
{
    CHECK_RETURN(InResource == resource)
    auto res = InResource.Get();
    CHECK_RETURN(!res);
    auto& info = res->data;
    CHECK_RETURN(info.ID.Get().empty());

    resource = InResource;
    
    String name = info.Name;
    String creator = info.CreatorName;

    if (name.empty())
        name = "Untitled";
    if (creator.empty())
        creator = "you";
    
    Vec3I size = info.Size;
    int objects = info.Objects;
    float length = info.Length;
    int spawners = info.Spawners;
    
    SetText("Name", name);
    Get<Textbox>("NameEdit").SetText(info.Name);
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
