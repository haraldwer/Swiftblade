#pragma once

#include "ECS/Player/PlayerInterface.h"
#include "Engine/ECS/UniqueComponent.h"
#include "States.h"

class AnimationStateMachine;

namespace ECS
{
    class Animator : public UniqueComponent<Animator>, public PlayerInterface
    {
    public:

        void Init() override;
        void Update() override;
        void TryOverrideState(const Type& InAnimState) const;
        bool EditState() const;

        void SetHands(const HandState& InRight, const HandState& InLeft);
        void SetHead(const HeadState& InHead);

        HandState GetLeft() const { return CurrentLeft; }
        HandState GetRight() const { return CurrentRight; }
        HeadState GetHead() const { return CurrentHead; }

        Mat4F GetPose(const String& InName) const;
        Mat4F ToCameraSpace(Mat4F InMat, float InWeight);
        static HandState Flip(const HandState& InHand);
        
    private:
        
        void UpdateHands();
        void UpdateHead();
        
        ObjectPtr<AnimationStateMachine> StateMachine; 

        HandState TargetLeft;
        HandState TargetRight;
        HeadState TargetHead;

        HandState CurrentLeft;
        HandState CurrentRight;
        HeadState CurrentHead;
    };
}
