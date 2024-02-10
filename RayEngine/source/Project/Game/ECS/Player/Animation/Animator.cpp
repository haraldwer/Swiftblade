#include "Animator.h"

#include "AnimationStateMachine.h"
#include "Engine/Profiling/Profile.h"

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


