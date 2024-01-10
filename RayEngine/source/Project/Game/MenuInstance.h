#pragma once
#include "Engine/Instance/Instance.h"
#include "Engine/UI/Instance.h"

class MenuInstance : public Engine::Instance 
{
public: 
    void Init() override;
    void Update(double InDelta) override;
    void Draw() override;
};
