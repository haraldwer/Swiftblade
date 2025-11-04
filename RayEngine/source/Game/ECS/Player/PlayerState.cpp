#include "PlayerState.h"

#include "ECS/Player/Animation/PlayerAnimator.h"

void PlayerStateBase::ApplyAnimationState() const
{
    GetAnimator().TryOverrideState(GetAnimationState());
}