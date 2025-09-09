#pragma once
#include "Instance/Instance.h"

class MenuRoomBrowser : Menu::Instance
{
    TYPE_INFO(MenuRoomBrowser, Menu::Instance);
public:
    void Init() override;
    bool IsBlocking() const override { return true; }
};

