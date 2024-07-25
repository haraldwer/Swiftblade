#pragma once

class AnimationStateSlide : public AnimationState<AnimationStateSlide>
{
public:
    Type Update(double InDT) override;
    
};
