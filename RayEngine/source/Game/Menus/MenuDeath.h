#pragma once
#include "Engine/Instance/Instance.h"

class MenuDeath : Menu::Instance
{
public:
    void Init() override;
    void Update(double InDelta) override;
};
