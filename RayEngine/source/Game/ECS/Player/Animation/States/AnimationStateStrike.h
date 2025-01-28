#pragma once

class AnimationStateStrike : AnimationState<AnimationStateStrike>
{
public:
    Utility::Type Update() override;

private:
    PROPERTY_C(float, Duration, 2.0f);
};
