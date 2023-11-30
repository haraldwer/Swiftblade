#pragma once
#include "Engine/Instance/Instance.h"
#include "Engine/UI/Instance.h"

class Menu : public Engine::Instance 
{
public: 
    void Init() override;
    void Deinit() override;
    void Update(double InDelta) override;
    void UpdateUI() override;
private: 
    UI::Instance UI;
};
