#include "AnimationStateDefault.h"

#include "ECS/Player/Animation/Animator.h"
#include "ECS/Player/Animation/States.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"

Type AnimationStateDefault::Update()
{
    constexpr float frequency = 12.0f;
    
    ECS::Animator& a = GetAnimator();
    HeadState head = a.GetHead();
    Vec2F headBob = a.HeadBob(0.02f, frequency);
    head.Tilt = headBob.x;
    head.Position = Vec3F::Up() * headBob.y; 
    head.Interp = 20.0f;
    a.SetHead(head);

    Mat4F idle = a.GetPose("Pose_Idle");

    HandState right;
    right.Interp = 10.0f;
    right.Pose = HandPose::OPEN;
    right.Transform = idle;
    HandState left = a.Flip(right);

    // Also multiply with hvel
    
    
    right.Transform = a.HandBob(right.Transform, Vec2F(0.2f, 0.1f), frequency, true);
    left.Transform = a.HandBob(left.Transform, Vec2F(0.2f, 0.1f), frequency, false);
    
    a.SetHands(right, left);
    
    return Type::None(); 
}
