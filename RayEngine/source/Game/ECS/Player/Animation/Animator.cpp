#include "Animator.h"

#include "AnimationPoser.h"
#include "AnimationStateMachine.h"
#include "ECS/Player/Player.h"
#include "ECS/Systems/Transform.h"
#include "Engine/Profiling/Profile.h"
#include "ImGui/imgui.h"

void ECS::Animator::Init()
{
    StateMachine = new AnimationStateMachine();
    StateMachine->LoadConfig();
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

Mat4F ECS::Animator::ToCameraSpace(Mat4F InMat, float InWeight)
{
    auto& t = GetCameraTransform();
    auto cam = t.Local();
    auto camSpace = InMat;
    
    // 1. Offset with camera pos
    // 2. Apply camera rotation
    // 3. Return to original pos
    camSpace.SetPosition(camSpace.GetPosition() - cam.GetPosition());
    camSpace = camSpace * cam.GetRotationMatrix();
    camSpace.SetPosition(camSpace.GetPosition() + cam.GetPosition());
    return Mat4F::Lerp(InMat, camSpace, InWeight);
}

HandState ECS::Animator::Flip(const HandState& InHand)
{
    HandState result = InHand;
    Mat4F trans = Mat4F(InHand.Position, InHand.Rotation, Vec3F::One());
    trans = trans * Mat4F(Vec3F::Zero(), QuatF::Identity(), Vec3F(-1, 1, 1));
    result.Position = trans.GetPosition();
    result.Rotation = trans.GetRotation();
    return result;
}

void ECS::Animator::UpdateHands()
{
    // TODO: Interp
    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    CurrentRight.LerpTo(TargetRight, dt);
    CurrentLeft.LerpTo(TargetLeft, dt);

    const Mat4F r = Mat4F(CurrentRight.Position, CurrentRight.Rotation, Vec3F::One());
    const Mat4F l = Mat4F(CurrentLeft.Position, CurrentLeft.Rotation, Vec3F::One());
    const Mat4F cr = ToCameraSpace(r, CurrentRight.CameraSpace);
    const Mat4F cl = ToCameraSpace(l, CurrentLeft.CameraSpace);
    GetRightTransform().SetLocal(cr);
    GetLeftTransform().SetLocal(cl);
    if (auto t = TryGet<Transform>(GetPlayer().GetWeaponID()))
        t->SetLocal(cr);
}

void ECS::Animator::UpdateHead()
{
    // TODO: Interp
    CurrentHead = TargetHead;
    auto& camTrans = GetCameraTransform();
    const QuatF localRot = camTrans.GetRotation(Transform::Space::LOCAL);
    const Vec3F eulerRot = localRot.Euler();
    camTrans.SetRotation(
        QuatF::FromEuler(Vec3F(eulerRot.x, eulerRot.y, CurrentHead.Tilt)),
        Transform::Space::LOCAL);
    camTrans.SetPosition(
        Vec3F::Up() + CurrentHead.Position,
        Transform::Space::LOCAL);
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