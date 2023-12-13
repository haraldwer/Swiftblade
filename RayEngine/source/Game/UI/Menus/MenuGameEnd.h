#pragma once
#include "Engine/Menu/Menu.h"

class MenuGameEnd : Menu::Instance
{
public:
    void Init() override;
    void Update(double InDelta) override;
};
