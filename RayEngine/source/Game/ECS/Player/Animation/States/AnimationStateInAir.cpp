#include "AnimationStateInAir.h"

#include "ECS/Player/Animation/PlayerAnimator.h"
#include "ECS/Player/Animation/States.h"
#include "ECS/Systems/Rigidbody.h"

Type AnimationStateInAir::Update()
{
    auto& a = GetAnimator();

    HeadState head;
    head.interp = 10.0f;
    a.SetHead(head);
    
    HandState hands;
    hands.interp = 20.0f;
    hands.transform = a.GetPose("Pose_InAir");
    hands.velocityOffset.y *= 3.0f;
    hands.pose = HandPose::OPEN;
    hands.cameraSpace = 0.5;
    const HandState flipped = ECS::PlayerAnimator::Flip(hands);
    a.SetHands(hands, flipped);
    
    return Type::None(); 
}
