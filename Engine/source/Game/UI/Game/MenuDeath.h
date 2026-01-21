#pragma once
#include "Engine/Instance/Instance.h"

class MenuDeath : Menu::Instance
{
    CLASS_INFO(MenuDeath, Menu::Instance);
public:
    void Init() override;
    void Deinit() override;
    void Update() override;
};
