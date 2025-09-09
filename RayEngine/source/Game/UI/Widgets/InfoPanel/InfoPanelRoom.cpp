#include "InfoPanelRoom.h"

#include "Editor/RoomEditor.h"
#include "Instance/Manager.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"

void UI::InfoPanelRoom::Init(Container &InOwner)
{
    Container::Init(InOwner);

    Builder b = Builder(Container({
                .alignment = 1,
                .margins = 10
            }))
        .Push(List())
            .Add(LabelHeader(), "Name")
            .Add(LabelText(), "Creator")
            .Add(LabelText(), "Size")
            .Add(LabelText(), "Objects")
            .Add(LabelText(), "Enemies")
            .Add(LabelText(), "Status")
            .Add(LabelText(), "Date")
        .Pop()
        .Add(Label(
                {
                    .alignment = 1,
                    .anchor = {1, 1},
                    .pivot = {1, 1}},
                { "Open", 30, { 1, 1 } }),
            "Edit");
    Add(b.Build());
}

void UI::InfoPanelRoom::Update(Container &InOwner)
{
    Container::Update(InOwner);

    if (Get<Label>("Edit").IsClicked())
        if (auto editor = Engine::Manager::Get().Push<RoomEditor>())
            editor->SetRoom(room);
}

void UI::InfoPanelRoom::SetRoom(const ResRoom &InRoom)
{   
    auto res = InRoom.Get();
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
        auto& r = res->Get();
        SetText("Name", r.Name);
    }
}

void UI::InfoPanelRoom::SetText(const String &InEntry, const String &InText)
{
    Get<Label>(InEntry).SetText(InText);
}
