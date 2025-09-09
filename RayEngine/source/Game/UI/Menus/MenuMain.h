#pragma once
#include "Engine/Menu/Menu.h"

class MenuMain : Menu::Instance
{
    TYPE_INFO(MenuMain, Menu::Instance);
public:
    void Init() override;
    void Update() override;
};
