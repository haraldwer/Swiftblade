#include "AnimationStateDefault.h"

#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"

Type AnimationStateDefault::Update()
{
    const float scale = 0.02f; 
    const float height = 0.3f;
    const float frequency = 12.0f;
    const float interpSpeed = 30.0f;
    
    const float hSpeed = (GetRB().GetVelocity() * Vec3F(1.0f, 0.0f, 1.0f)).Length();
    const float alpha = Utility::Math::Min(hSpeed * 0.1f, 1.0f);
    const float time = static_cast<float>(Utility::Time::Get().Total());
    const float curve = sinf(time * frequency);
    
    const float desiredRoll = curve * alpha * scale;
    float dt = static_cast<float>(Utility::Time::Get().Delta());
    const float lerp = Utility::Math::Min(1.0f, interpSpeed * dt);
    CurrentRoll = Utility::Math::Lerp(CurrentRoll, desiredRoll, lerp); 
    
    auto& camTrans = GetCameraTransform();
    const QuatF localRot = camTrans.GetRotation(ECS::Transform::Space::LOCAL);
    const Vec3F eulerRot = localRot.Euler();
    camTrans.SetRotation(
        QuatF::FromEuler(Vec3F(eulerRot.x, eulerRot.y, CurrentRoll)),
        ECS::Transform::Space::LOCAL);
    
    const float desiredHeight = (1.0f - abs(curve)) * height * alpha;
    CurrentHeight = Utility::Math::Lerp(CurrentHeight, desiredHeight, lerp); 
    camTrans.SetPosition(
        Vec3F::Up() * (1.0f + CurrentHeight),
        ECS::Transform::Space::LOCAL);
    
    return Type::None(); 
}
