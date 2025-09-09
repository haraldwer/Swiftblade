#include "InfoPanel.h"

#include "InfoPanelLevel.h"
#include "InfoPanelRoom.h"
#include "Editor/RoomEditor.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/TabContainer.h"

void UI::InfoPanel::Init(Container &InOwner)
{
    Container::Init(InOwner);
    
    auto b = Builder()
        .Push(TabContainer(Transform::Fill()), "Tab")
            .Add(InfoPanelLevel(), "Level")
            .Add(InfoPanelRoom(), "Room");
    Add(b.Build());
    
    SetBackground({
        .color = { 1, 1, 1, 0.5 }
    });

    onLevelSelected.Bind([](auto l, auto c) { c->SetLevel(l.entry); });
    onRoomSelected.Bind([](auto r, auto c) { c->SetRoom(r.room); });
}

void UI::InfoPanel::SetRoom(const ResRoom& InRoom)
{
    LOG("Room selected: " + InRoom.Identifier().Str())
    Get<TabContainer>("Tab").Set("Room");
    Get<InfoPanelRoom>("Room").SetRoom(InRoom);
}

void UI::InfoPanel::SetLevel(const DB::RPCLevelList::Entry& InLevel)
{
    LOG("Level selected: " + InLevel.Name.Get())
    Get<TabContainer>("Tab").Set("Level");
    Get<InfoPanelLevel>("Level").SetLevel(InLevel);
}
