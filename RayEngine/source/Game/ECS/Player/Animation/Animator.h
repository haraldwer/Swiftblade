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

        HandState GetLeft() const { return currentLeft; }
        HandState GetRight() const { return currentRight; }
        HeadState GetHead() const { return currentHead; }

        Mat4F GetPose(const String& InName) const;
        Mat4F ToCameraSpace(const Mat4F& InMat, float InWeightconst) const;
        static HandState Flip(const HandState& InHand);

        Vec2F HeadBob(float InScale = 0.02f, float InFrequency = 12.0f, float InHeight = 0.1f, float InLeaning = -0.05) const;
        Mat4F HandBob(Mat4F InTrans, Vec2F InScale, float InFrequency, bool InRight) const;
        
    private:
        
        void UpdateHands();
        void UpdateHead();
        
        ObjectPtr<AnimationStateMachine> stateMachine; 

        HandState targetLeft;
        HandState targetRight;
        HeadState targetHead;

        HandState currentLeft;
        HandState currentRight;
        HeadState currentHead;
    };
}
