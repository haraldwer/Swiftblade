#include "Animator.h"

#include "AnimationPoser.h"
#include "AnimationStateMachine.h"
#include "ECS/Player/Player.h"
#include "ECS/Player/PlayerCamera.h"
#include "ECS/Systems/Rigidbody.h"
#include "ECS/Systems/Transform.h"
#include "Engine/Profiling/Profile.h"
#include "ImGui/imgui.h"

void ECS::Animator::Init()
{
    if (Engine::Instance::Get().IsEditor())
        return;
    
    StateMachine = new AnimationStateMachine();
    StateMachine->Init();
}

void ECS::Animator::Update()
{
    PROFILE_SCOPE_BEGIN("StateMachine");
    if (StateMachine)
        StateMachine->Update();
    PROFILE_SCOPE_END();
    UpdateHead();
    UpdateHands();
}

void ECS::Animator::TryOverrideState(const Type& InAnimState) const
{
    if (StateMachine)
        StateMachine->TryOverrideState(InAnimState);
}

bool ECS::Animator::EditState() const
{
    if (!StateMachine)
        return false;
    if (ImGui::Button("Save##Animator"))
        StateMachine->SaveConfig();
    return StateMachine->Edit();
}

Mat4F ECS::Animator::GetPose(const String& InName) const
{
    if (auto w = GetWeaponPoser()) // Prioritize weapons
    {
        Mat4F pose = w->GetPose(InName);
        if (pose != Mat4F())
            return pose;
    }
    return GetHandPoser().GetPose(InName);
}

Mat4F ECS::Animator::ToCameraSpace(const Mat4F& InMat, float InWeight) const
{
    const Mat4F cam = GetCameraTransform().Local();
    Mat4F camSpace = InMat;
    
    camSpace.translation.xyz -= cam.GetPosition();
    camSpace = camSpace * cam.GetRotationMatrix();
    camSpace.translation.xyz += cam.GetPosition();
    return Mat4F::Lerp(InMat, camSpace, InWeight);
}

HandState ECS::Animator::Flip(const HandState& InHand)
{
    HandState result = InHand;
    result.Transform *= Mat4F(Vec3F::Zero(), QuatF::Identity(), Vec3F(-1, 1, 1));
    return result;
}

Vec2F ECS::Animator::HeadBob(float InScale, float InFrequency, float InHeight, float InLeaning) const
{
    const Vec3F vel = GetRB().GetVelocity();
    const float hSpeed = (vel * Vec3F(1.0f, 0.0f, 1.0f)).Length();
    const float alpha = Utility::Math::Min(hSpeed * 0.05f, 1.0f);
    const float time = static_cast<float>(Utility::Time::Get().Total());
    const float curve = sinf(time * InFrequency);
    
    float desiredRoll = curve * alpha * InScale;
    desiredRoll += alpha * Vec3F::Dot(vel.GetNormalized(), GetPlayerTransform().World().Right()) * InLeaning;

    const float desiredHeight = (1.0f - abs(curve)) * InHeight * alpha;
    return { desiredRoll, desiredHeight };
}

Mat4F ECS::Animator::HandBob(Mat4F InTrans, Vec2F InScale, float InFrequency, bool InRight) const
{
    const Vec3F vel = GetRB().GetVelocity();
    
    const float hSpeed = (vel * Vec3F(1.0f, 0.0f, 1.0f)).Length();
    const float alpha = Utility::Math::Min(hSpeed * 0.05f, 1.0f);
    const float time = static_cast<float>(Utility::Time::Get().Total());
    const float curve = sinf(time * InFrequency + (InRight ? 0.0f : PI_FLOAT));

    InTrans.translation.xyz += Vec3F::Forward() * curve * alpha * InScale.x;
    InTrans.translation.xyz += Vec3F::Up() * curve * alpha * InScale.y;
    
    return InTrans;
}

void ECS::Animator::UpdateHands()
{
    HandState tR = TargetRight;
    HandState tL = TargetLeft;

    const Vec3F vel = GetRB().GetVelocity();
    tR.Transform.translation.xyz -= vel * tR.VelocityOffset * 0.01f;
    tL.Transform.translation.xyz -= vel * tL.VelocityOffset * 0.01f;
    
    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    CurrentRight.LerpTo(tR, dt);
    CurrentLeft.LerpTo(tL, dt);

    const Mat4F cr = ToCameraSpace(CurrentRight.Transform, CurrentRight.CameraSpace);
    const Mat4F cl = ToCameraSpace(CurrentLeft.Transform, CurrentLeft.CameraSpace);
    GetRightTransform().SetLocal(cr);
    GetLeftTransform().SetLocal(cl);
    if (auto t = TryGet<Transform>(GetPlayer().GetWeaponID()))
        t->SetLocal(cr);
}

void ECS::Animator::UpdateHead()
{
    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    CurrentHead.LerpTo(TargetHead, dt);
    
    auto& camTrans = GetCameraTransform();
    const QuatF localRot = camTrans.GetRotation(Transform::Space::LOCAL);
    const Vec3F eulerRot = localRot.Euler();
    Mat4F trans = Mat4F(
        Vec3F::Up() + CurrentHead.Position,
        QuatF::FromEuler(Vec3F(eulerRot.x, eulerRot.y, CurrentHead.Tilt)),
        Vec3F::One());
    GetPlayerCamera().SetTransform(trans);
}

void ECS::Animator::SetHands(const HandState& InRight, const HandState& InLeft)
{
    TargetRight = InRight;
    TargetLeft = InLeft;
}

void ECS::Animator::SetHead(const HeadState& InHead)
{
    TargetHead = InHead;
}