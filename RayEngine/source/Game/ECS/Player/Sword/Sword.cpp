#include "Sword.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Utility/Time/Time.h"

void Sword::Init()
{
    StartPos = Get<ECS::Transform>().GetPosition(); 
}

void Sword::Update(double InDelta)
{
    auto& t = Get<ECS::Transform>();
    if (t.GetParent() == ECS::InvalidID)
    {
        const float time = static_cast<float>(Utility::Time::Get().Total());
        t.SetPosition(StartPos + Vec3F::Up() * sin(time * FloatSpeed) * FloatDist.Get());
        t.SetRotation(Vec3F(0.0f, time * RotationSpeed, 0.0f)); 
    }
}
