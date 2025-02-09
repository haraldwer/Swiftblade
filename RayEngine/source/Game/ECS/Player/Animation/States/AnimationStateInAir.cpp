#include "AnimationStateInAir.h"

#include "ECS/Player/Animation/Animator.h"
#include "ECS/Player/Animation/States.h"
#include "ECS/Systems/Rigidbody.h"

Type AnimationStateInAir::Update()
{
    auto& a = GetAnimator();

    HeadState head;
    head.Interp = 10.0f;
    a.SetHead(head);
    
    HandState hands;
    hands.Interp = 20.0f;
    hands.Transform = a.GetPose("Pose_InAir");
    hands.VelocityOffset.y *= 3.0f;
    hands.Pose = HandPose::OPEN;
    HandState flipped = ECS::Animator::Flip(hands);
    a.SetHands(hands, flipped);
    
    return Type::None(); 
}
