#include "PlayerCamera.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Manager.h"

void ECS::PlayerCamera::Init()
{
    if (Engine::Instance::Get().IsEditor())
        return;
    
    OriginalPosition = GetCameraTransform().GetPosition(Transform::Space::LOCAL); 
}

void ECS::PlayerCamera::Update()
{
    // Interpolate crouch offset
    float dt = static_cast<float>(Utility::Time::Get().Delta());
    CrouchOffset = Utility::Math::Lerp(CrouchOffset, TargetCrouchOffset, dt * CrouchInterpSpeed);

    auto& camTrans = GetCameraTransform();
    camTrans.SetPosition(OriginalPosition + Vec3F::Up() * CrouchOffset, Transform::Space::LOCAL); 
}

void ECS::PlayerCamera::AddCrouchOffset(float InHeightDiff)
{
    // Move camera up a bit
    // And move reference down

    TargetCrouchOffset = InHeightDiff * CrouchOffsetMultiplier;
    CrouchOffset -= InHeightDiff * 0.5f;
}

void ECS::PlayerCamera::SetTransform(const Mat4F& InTrans)
{
    OriginalPosition = InTrans.GetPosition();
    
    auto& camTrans = GetCameraTransform();
    camTrans.SetPosition(OriginalPosition + Vec3F::Up() * CrouchOffset, Transform::Space::LOCAL);
    camTrans.SetRotation(InTrans.GetRotation(), Transform::Space::LOCAL);
}
