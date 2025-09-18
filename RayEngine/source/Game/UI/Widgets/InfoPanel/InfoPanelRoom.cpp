#include "InfoPanelRoom.h"

#include "Editor/RoomEditor.h"
#include "Instance/Manager.h"
#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/Label.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"
#include "UI/Widgets/RoomList/RoomEntryWidget.h"

void UI::InfoPanelRoom::Init(Container &InOwner)
{
    Container::Init(InOwner);

    Builder b = Builder(Container({
                .alignment = 1,
                .margins = 10
            }))
        .Push(List())
            .Push(Container())
                //.Add(Image({ .size = { 0, 200 }, .alignment = { 1, 0 }}))
                .Push(List({}, { .direction = ListDirection::HORIZONTAL }))
                    .Add(ButtonDefault({ .anchor = 0.5, .pivot = 0.5 }, "EDIT"), "EditName")
                    .Push(List({ .anchor = { 0, 0.5 }, .pivot = { 0, 0.5 }}))
                        .Add(LabelHeader(), "Name")
                        .Add(LabelText({.padding = {0, {-5, 0}}}), "Creator")
                    .Pop()
                .Pop()
            .Pop()
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
}

void UI::InfoPanelRoom::Update(Container &InOwner)
{
    Container::Update(InOwner);

    if (Get<ButtonDefault>("Open").IsClicked())
        if (auto editor = Engine::Manager::Get().Push<RoomEditor>())
            editor->SetRoom(room);
}

void UI::InfoPanelRoom::SetRoom(const RoomEntrySelected &InRoom)
{
    room = InRoom.room;
    auto res = InRoom.room.Get();
    if (!res)
    {
        // Assume new room
        SetText("Name", "Unknown");
        SetText("Creator", "by you");
        SetText("Size", "Size: -");
        SetText("Objects", "Objects: -");
        SetText("Enemies", "Enemies: -");
        SetText("Status", "Status: -");
        SetText("Date", "Date: -");
    }
    else
    {
        // Use submit info?
        SetText("Name", res->data.Name);
        SetText("Creator", "by you");
        SetText("Size", "Size: -");
        SetText("Objects", "Objects: -");
        SetText("Enemies", "Enemies: -");
        SetText("Status", "Status: -");
        SetText("Date", "Date: -");
    }
}

void UI::InfoPanelRoom::SetText(const String &InEntry, const String &InText)
{
    Get<Label>(InEntry).SetText(InText);
}
