#include "AnimationStateIdle.h"

#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Game/ECS/Player/Movement/Movement.h"
#include "Utility/Time/Time.h"

Type AnimationStateIdle::Update(double InDT)
{
    const float speed = GetRB().GetVelocity().Length();
    const bool onGround = GetMovement().IsOnGround();
    
    const float alpha = MIN(speed * 0.1f, 1.0f) * static_cast<float>(onGround);
    const float scale = 0.05f; 
    const float frequency = 10.0f;
    const float time = static_cast<float>(Utility::Time::Get().Total());

    const float interpSpeed = 10.0f; 
    const float desiredRoll = sinf(time * frequency) * alpha * scale;
    const float lerp = MIN(1.0f, interpSpeed * InDT);
    CurrentRoll = LERP(CurrentRoll, desiredRoll, lerp); 
    
    auto& camTrans = GetCameraTransform();
    const QuatF localRot = camTrans.GetRotation(ECS::Transform::Space::LOCAL);
    const Vec3F eulerRot = localRot.Euler();
    camTrans.SetRotation(
        QuatF::FromEuler(
            Vec3F(
                eulerRot.x,
                eulerRot.y,
                CurrentRoll)),
        ECS::Transform::Space::LOCAL);
    
    return Type::None(); 
}
