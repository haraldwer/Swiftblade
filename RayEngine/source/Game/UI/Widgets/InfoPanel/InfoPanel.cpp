#include "InfoPanel.h"

#include "InfoPanelEditRoom.h"
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
            .Add(InfoPanelEditRoom(), "EditRoom");
    Add(b.Build());
    
    SetBackground({
        .color = { 1, 1, 1, 0.5 }
    });

    onLevelSelected.Bind([](auto l, auto c) { c->SetLevel(l); });
    onEditRoomSelected.Bind([](auto r, auto c) { c->SetEditRoom(r); });
}

void UI::InfoPanel::SetEditRoom(const EditRoomEntryData& InEvent)
{
    CHECK_RETURN(InEvent.add);
    Get<TabContainer>("Tab").Set("EditRoom");
    Get<InfoPanelEditRoom>("EditRoom").SetRoom(InEvent.resource);
}

void UI::InfoPanel::SetLevel(const LevelEntryData& InEvent)
{
    Get<TabContainer>("Tab").Set("Level");
    Get<InfoPanelLevel>("Level").SetLevel(InEvent);
}
