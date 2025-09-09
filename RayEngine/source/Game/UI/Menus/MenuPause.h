#pragma once
#include "Instance/Instance.h"

class MenuPause : public Menu::Instance
{
    TYPE_INFO(MenuPause, Menu::Instance);
public:
    void Init() override;
    void Update() override;
    void Deinit() override;
};
