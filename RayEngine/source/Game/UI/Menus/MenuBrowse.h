#pragma once
#include "Instance/Instance.h"
#include "UI/Widgets/BrowsePanel.h"

class MenuBrowse : public Menu::Instance
{
public:
    void Init() override;
    void Update() override;
    
    void SetPanelType(const Type& InType) { panelType = InType; }

private:
    Type panelType;
    UI::LevelSelectCallback onLevelSelected;
    UI::RoomSelectCallback onRoomSelected;
    
};
