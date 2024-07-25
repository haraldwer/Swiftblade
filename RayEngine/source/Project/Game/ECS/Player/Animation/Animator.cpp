#include "Animator.h"

#include "AnimationStateMachine.h"
#include "Engine/Profiling/Profile.h"
#include "Game/ECS/Player/Movement/Movement.h"

void Animator::Init()
{
    StateMachine = new AnimationStateMachine();
    StateMachine->Init();
}

void Animator::Update(double InDelta)
{
    PROFILE_SCOPE_BEGIN("StateMachine");
    if (StateMachine)
        StateMachine->Update(InDelta);
    PROFILE_SCOPE_END();
}

void Animator::TryOverrideState(const Type& InAnimState) const
{
    if (StateMachine)
        StateMachine->TryOverrideState(InAnimState);
}


