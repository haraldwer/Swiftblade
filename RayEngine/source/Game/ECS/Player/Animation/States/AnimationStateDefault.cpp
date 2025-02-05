#include "AnimationStateDefault.h"

#include "ECS/Player/Animation/Animator.h"
#include "ECS/Player/Animation/States.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"

Type AnimationStateDefault::Update()
{
    const float scale = 0.02f; 
    const float height = 0.1f;
    const float frequency = 12.0f;
    const float leaning = -0.05f;

    const Vec3F vel = GetRB().GetVelocity();
    const float hSpeed = (vel * Vec3F(1.0f, 0.0f, 1.0f)).Length();
    const float alpha = Utility::Math::Min(hSpeed * 0.05f, 1.0f);
    const float time = static_cast<float>(Utility::Time::Get().Total());
    const float curve = sinf(time * frequency);
    
    float desiredRoll = curve * alpha * scale;
    const float desiredHeight = (1.0f - abs(curve)) * height * alpha;

    desiredRoll += alpha * vel.GetNormalized().Dot(GetPlayerTransform().World().Right()) * leaning;

    auto& a = GetAnimator();
    HeadState head = a.GetHead();
    head.Tilt = desiredRoll;
    head.Position = Vec3F::Up() * desiredHeight; 
    head.Interp = 20.0f;
    a.SetHead(head);

    Mat4F idle = a.GetPose("Pose_Idle");
    HandState hands;
    hands.Interp = 10.0f;
    hands.Pose = HandPose::CLOSED;
    hands.Position = idle.GetPosition();
    hands.Rotation = idle.GetRotation();
    HandState flipped = ECS::Animator::Flip(hands);
    Vec3F velOff = GetRB().GetVelocity() * (Vec3F::Up() * 0.01f + Vec3F::Right() * 0.01f);
    hands.Position -= velOff;
    flipped.Position -= velOff;
    a.SetHands(hands, flipped);
    
    return Type::None(); 
}
