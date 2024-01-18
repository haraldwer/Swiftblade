#include "Weapon.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Rendering/Debug/Draw.h"
#include "Utility/Time/Time.h"

void Weapon::Init()
{
    StartPos = Get<ECS::Transform>().GetPosition(); 
}

void Weapon::Update(double InDelta)
{
    auto& t = Get<ECS::Transform>();
    if (t.GetParent() == ECS::InvalidID)
    {
        const float time = static_cast<float>(Utility::Time::Get().Total());
        t.SetPosition(StartPos + Vec3F::Up() * sin(time * FloatSpeed) * FloatDist.Get());
        t.SetRotation(QuatF::FromEuler(Vec3F(0.0f, time * RotationSpeed, 0.0f)));
    }
}

