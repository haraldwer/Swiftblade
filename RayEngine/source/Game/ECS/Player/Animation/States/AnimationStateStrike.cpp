#include "AnimationStateStrike.h"

Utility::Type AnimationStateStrike::Update()
{
    double time = GetTimeSinceEnter();
    if (time > Duration)
        return Type::Get<AnimationStateDefault>();

    auto& a = GetAnimator();
    Mat4F start = a.GetPose("Pose_Strike_Start");
    Mat4F end = a.GetPose("Pose_Strike_End");

    float lerp = time / Duration;
    Mat4F result = Mat4F::Lerp(start, end, lerp);

    HandState state;
    state.Interp = 0;
    state.Pose = HandPose::CLOSED;
    state.Position = result.GetPosition();
    state.Rotation = result.GetRotation();
    
    a.SetHead({});
    a.SetHands(state, state);
    
    return GetType(); 
}
