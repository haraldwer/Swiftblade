#pragma once
#include "Instance/Instance.h"

class MenuRoomInfo : Menu::Instance
{
    TYPE_INFO(MenuRoomInfo, Menu::Instance);
public:
    void Init() override;
    bool IsBlocking() const override { return true; }
};


