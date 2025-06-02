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
    head.tilt = headBob.x;
    head.position = Vec3F::Up() * headBob.y; 
    head.interp = 5.0f;
    a.SetHead(head);

    Mat4F idle = a.GetPose("Pose_Idle");

    HandState right;
    right.interp = 10.0f;
    right.pose = HandPose::OPEN;
    right.transform = idle;
    HandState left = a.Flip(right);

    // Also multiply with hvel
    
    
    right.transform = a.HandBob(right.transform, Vec2F(0.2f, 0.1f), frequency, true);
    left.transform = a.HandBob(left.transform, Vec2F(0.2f, 0.1f), frequency, false);
    
    a.SetHands(right, left);
    
    return Type::None(); 
}
