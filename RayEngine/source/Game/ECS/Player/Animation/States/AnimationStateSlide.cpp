#include "AnimationStateSlide.h"

#include "ECS/Player/Animation/Animator.h"
#include "ECS/Player/Animation/States.h"

Type AnimationStateSlide::Update()
{
    auto& a = GetAnimator();
    
    HeadState head;
    head.Interp = 5.0f;
    head.Tilt = 0.02f;
    a.SetHead(head);
    
    Mat4F slide = a.GetPose("Pose_Slide_R");

    HandState right;
    right.Interp = 10.0f;
    right.Pose = HandPose::OPEN;
    right.Transform = slide;

    HandState left;
    left.Interp = 10.0f;
    left.Pose = HandPose::OPEN;
    left.Transform = slide;

    a.SetHands(right, left);
    
    return Type::None(); 
}
