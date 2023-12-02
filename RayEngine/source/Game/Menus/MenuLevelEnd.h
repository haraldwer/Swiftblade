#pragma once
#include "Engine/Menu/Menu.h"

class MenuLevelEnd : Menu::Instance
{
public:
    void Init() override;
    void Update(double InDelta) override;
};
