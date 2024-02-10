#pragma once

#include "Engine/ECS/UniqueComponent.h"
#include "Game/ECS/Player/PlayerInterface.h"

class AnimationStateMachine;

class Animator : public ECS::UniqueComponent<Animator>, public ECS::PlayerInterface
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
    void Update(double InDelta) override;
    
private: 
    
    ObjectPtr<AnimationStateMachine> StateMachine; 
    
};
