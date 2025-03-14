#include "Weapon.h"

#include "Engine/ECS/Systems/Transform.h"

void ECS::Weapon::Init()
{
    StartPos = Get<Transform>().GetPosition(); 
}

void ECS::Weapon::Update()
{
    auto& t = Get<Transform>();
    if (t.GetParent() == InvalidID)
    {
        const float time = static_cast<float>(Utility::Time::Get().Total());
        t.SetPosition(StartPos + Vec3F::Up() * sin(time * FloatSpeed) * FloatDist.Get());
        t.SetRotation(QuatF::FromEuler(Vec3F(0.0f, time * RotationSpeed, 0.0f)));
    }
}


