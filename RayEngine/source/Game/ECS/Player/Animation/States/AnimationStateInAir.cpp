#include "AnimationStateInAir.h"

Type AnimationStateInAir::Update()
{
    auto& a = GetAnimator();

    HeadState head;
    head.Interp = 10.0f;
    a.SetHead(head);
    
    Mat4F inAir = a.GetPose("Pose_InAir");
    HandState hands;
    hands.Interp = 20.0f;
    hands.Pose = HandPose::CLOSED;
    hands.Position = inAir.GetPosition();
    hands.Rotation = inAir.GetRotation();
    HandState flipped = ECS::Animator::Flip(hands);
    Vec3F velOff = GetRB().GetVelocity() * (Vec3F::Up() * 0.03f + Vec3F::Right() * 0.01f);
    hands.Position -= velOff;
    flipped.Position -= velOff;
    a.SetHands(hands, flipped);
    
    return Type::None(); 
}
