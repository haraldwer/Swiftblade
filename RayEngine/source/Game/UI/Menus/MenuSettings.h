#pragma once
#include "Instance/Instance.h"

class MenuSettings : public Menu::Instance
{
    CLASS_INFO(MenuSettings, Menu::Instance);
public:
    void Init() override;
    void Update() override;
};
