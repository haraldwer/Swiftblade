#include "AnimationStateSlide.h"

#include "ECS/Player/Animation/PlayerAnimator.h"
#include "ECS/Player/Animation/States.h"

Type AnimationStateSlide::Update()
{
    auto& a = GetAnimator();
    
    HeadState head;
    head.interp = 5.0f;
    head.tilt = 0.02f;
    a.SetHead(head);
    
    Mat4F slide = a.GetPose("Pose_Slide_R");

    HandState right;
    right.interp = 10.0f;
    right.pose = HandPose::OPEN;
    right.transform = slide;
    right.cameraSpace = 0.3f; 
    HandState left = ECS::PlayerAnimator::Flip(right);

    a.SetHands(right, left);
    
    return Type::None(); 
}
