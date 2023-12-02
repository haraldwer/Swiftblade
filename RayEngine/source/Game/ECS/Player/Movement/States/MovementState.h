#pragma once

#include "Engine/Property/PropertyOwner.h"
#include "Game/ECS/Player/PlayerInterface.h"
#include "Utility/StateMachine/StateBase.h"

namespace Physics
{
    struct Contact;
}

template <class T>
class MovementState : public State<T>, public PropertyOwner<T>, public ECS::PlayerInterface
{
};
