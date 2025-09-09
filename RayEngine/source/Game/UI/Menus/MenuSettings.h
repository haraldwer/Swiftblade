#pragma once
#include "Instance/Instance.h"

class MenuSettings : public Menu::Instance
{
    TYPE_INFO(MenuSettings, Menu::Instance);
public:
    void Init() override;
    void Update() override;
};
