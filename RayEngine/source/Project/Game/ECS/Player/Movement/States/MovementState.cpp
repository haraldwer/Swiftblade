#include "MovementState.h"

#include "Game/ECS/Player/Animation/Animator.h"

void MovementStateBase::ApplyAnimationState() const
{
    GetAnimator().TryOverrideState(GetAnimationState());
}
