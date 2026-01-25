#include "PlayerCamera.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/InstanceManager.h"

void ECS::PlayerCamera::Init()
{
    if (Engine::Instance::Get().IsEditor())
        return;
    
    originalPosition = GetCameraTransform().GetPosition(Transform::Space::LOCAL); 
}

void ECS::PlayerCamera::Update()
{
    // Interpolate crouch offset
    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    crouchOffset = Utility::Math::Lerp(crouchOffset, targetCrouchOffset, dt * CrouchInterpSpeed);

    auto& camTrans = GetCameraTransform();
    camTrans.SetPosition(originalPosition + Vec3F::Up() * crouchOffset, Transform::Space::LOCAL); 
}

void ECS::PlayerCamera::AddCrouchOffset(const float InHeightDiff)
{
    // Move camera up a bit
    // And move reference down

    targetCrouchOffset = InHeightDiff * CrouchOffsetMultiplier;
    crouchOffset -= InHeightDiff * 0.5f;
}

void ECS::PlayerCamera::SetTransform(const Mat4F& InTrans)
{
    originalPosition = InTrans.GetPosition();
    
    auto& camTrans = GetCameraTransform();
    camTrans.SetPosition(originalPosition + Vec3F::Up() * crouchOffset, Transform::Space::LOCAL);
    camTrans.SetRotation(InTrans.GetRotation(), Transform::Space::LOCAL);
}
