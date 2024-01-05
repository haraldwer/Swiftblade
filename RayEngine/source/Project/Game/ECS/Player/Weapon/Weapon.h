#pragma once
#include "Engine/ECS/UniqueComponent.h"

class Weapon : public ECS::UniqueComponent<Weapon>
{
public:
    void Init() override;
    void Update(double InDelta) override;

private:

    PROPERTY_C(float, RotationSpeed, 0.7f);
    PROPERTY_C(float, FloatSpeed, 1.2f);
    PROPERTY_C(float, FloatDist, 0.5f);
    
    Vec3F StartPos; 
};
