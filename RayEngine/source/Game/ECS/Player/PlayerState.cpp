#include "PlayerState.h"

#include "ECS/Player/Animation/Animator.h"

void PlayerStateBase::ApplyAnimationState() const
{
    GetAnimator().TryOverrideState(GetAnimationState());
}