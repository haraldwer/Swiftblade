#include "AnimationStateStrike.h"

Utility::Type AnimationStateStrike::Update()
{
    double time = GetTimeSinceEnter();
    if (time > Duration)
        return Type::Get<AnimationStateDefault>();

    auto& a = GetAnimator();
    Mat4F start = a.GetPose("Pose_Strike_Start");
    Mat4F end = a.GetPose("Pose_Strike_End");

    double lerp = 1.0f - time / Duration;

    HandState state;
    state.Interp = 0;
    state.Pose = HandPose::CLOSED;
    state.Position = start.GetPosition();
    state.Rotation = start.GetRotation();
    
    a.SetHead({});
    a.SetHands(state, state);
    
    return {};
}
