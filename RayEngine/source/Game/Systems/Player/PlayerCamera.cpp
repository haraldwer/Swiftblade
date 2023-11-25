#include "PlayerCamera.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Manager.h"

void ECS::PlayerCamera::Init()
{
    if (Engine::Instance::Get().IsEditor())
        return;
    
    OriginalPosition = GetCameraTransform().GetPosition(Transform::Space::LOCAL); 
}

void ECS::PlayerCamera::Update(double InDelta)
{
    // Interpolate crouch offset

    CrouchOffset = LERP(CrouchOffset, TargetCrouchOffset, InDelta * CrouchInterpSpeed);
    const Vec3F offset = Vec3F::Up() * CrouchOffset;
    
    auto& camTrans = GetCameraTransform();
    camTrans.SetPosition(OriginalPosition + offset, Transform::Space::LOCAL); 
}

void ECS::PlayerCamera::AddCrouchOffset(float InHeightDiff)
{
    // Move camera up a bit
    // And move reference down

    TargetCrouchOffset = InHeightDiff * CrouchOffsetMultiplier;
    CrouchOffset -= InHeightDiff * 0.5f;
}
