#include "AnimationStateStrike.h"

#include "AnimationStateDefault.h"
#include "ECS/Player/Animation/Animator.h"
#include "ECS/Player/Animation/States.h"

Utility::Type AnimationStateStrike::Update()
{
    double time = GetTimeSinceEnter();
    if (time > Duration)
        return Type::Get<AnimationStateDefault>();

    auto& a = GetAnimator();
    Mat4F start = a.GetPose("Pose_Strike_Start");
    Mat4F end = a.GetPose("Pose_Strike_End");

    float lerp = static_cast<float>(time) / Duration;
    float lerpPow = pow(lerp, LerpPow.Get());
    Mat4F result = Mat4F::Lerp(start, end, lerpPow);

    HandState state;
    state.interp = 0;
    state.pose = HandPose::CLOSED;
    state.velocityOffset = Vec3F::Zero();
    state.transform = result;
    
    a.SetHead({});
    a.SetHands(state, state);
    
    return GetType(); 
}
