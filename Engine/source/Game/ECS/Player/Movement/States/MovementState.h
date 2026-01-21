#pragma once
#include "ECS/Player/PlayerState.h"
#include "ECS/Player/Movement/MovementParams.h"

template <class T>
class MovementState : public PlayerState<T>
{
};
