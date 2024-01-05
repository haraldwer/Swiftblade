#pragma once
#include "Engine/Instance/Instance.h"

class MenuLeaderboard : Menu::Instance
{
public:
    void Init() override;
    void Update(double InDelta) override;
};
