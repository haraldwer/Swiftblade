#pragma once
#include "ECS/Player/PlayerInterface.h"
#include "Property/PropertyOwner.h"
#include "Utility/StateMachine/StateBase.h"

template <class T> 
class CombatState : public State<T>, public ECS::PlayerInterface
{
};
