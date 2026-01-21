#pragma once
#include "Instance/Instance.h"
#include "Level/RoomInfo.h"
#include "UI/Widgets/RoomList/RoomEntryWidget.h"

class MenuRoomSelect : public Menu::Instance
{
    CLASS_INFO(MenuRoomSelect, Menu::Instance);
public:
    void Init() override;
    void Update() override;
    bool IsBlocking() const override { return true; }
    bool CanBePopped() const override { return false; }
    
    struct OnSelectEvent
    {
        UI::RoomEntryData entry;
    };
};
