#pragma once

#include "ECS/Player/PlayerInterface.h"
#include "Engine/ECS/UniqueComponent.h"
#include "States.h"

class AnimationStateMachine;

namespace ECS
{
    class Animator : public UniqueComponent<Animator>, public PlayerInterface
    {
        /*
        The players hands and head should move "realistically"
        The movement is based on the following factors:
            - Current movement state
            - Current combat state
        Each animation state should produce a desired hand and head state.

        Different weapons result in different animations.
        This means that the weapon has its own animation logic as well.

        Evaluation order:
         - Get weapon hand transform (defaults to identity)
         - Lerp target hand from weapon to current state transform (different states have different weight)
         - 

        Important feature:
        Being able to easily set up hand transforms
        */

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
