#pragma once
#include "Engine/ECS/UniqueComponent.h"
#include "Game/ECS/Player/PlayerInterface.h"

class Combat : public ECS::UniqueComponent<Combat>, public ECS::PlayerInterface
{
public:
    void Update() override;

private:
    PROPERTY_C(float, DamageSweepSize, 1.0f)
    
};
