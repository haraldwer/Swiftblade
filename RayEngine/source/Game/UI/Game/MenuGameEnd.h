#pragma once
#include "Engine/Menu/Menu.h"

class MenuGameEnd : Menu::Instance
{
    TYPE_INFO(MenuGameEnd, Menu::Instance);
public:
    void Init() override;
    void Deinit() override;
    void Update() override;
};
