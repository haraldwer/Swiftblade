#include "MovementState.h"

#include "ECS/Player/Animation/Animator.h"

void MovementStateBase::ApplyAnimationState() const
{
    GetAnimator().TryOverrideState(GetAnimationState());
}
