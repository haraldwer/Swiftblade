#pragma once
#include "Engine/Menu/Menu.h"

class MenuMain : Menu::Instance
{
    CLASS_INFO(MenuMain, Menu::Instance);
public:
    void Init() override;
    void Update() override;
};
