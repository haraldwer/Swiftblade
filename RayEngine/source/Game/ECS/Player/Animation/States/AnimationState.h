#pragma once

#include "ECS/Player/PlayerInterface.h"
#include "Engine/Utility/StateMachine/StateBase.h"
#include "Property/PropertyOwner.h"

template <class T> 
class AnimationState : public State<T>, public ECS::PlayerInterface
{
    double GetEnterTimestamp() const { return EnterTimestamp; }
    double GetExitTimestamp() const { return EnterTimestamp; }
    double GetTimeSinceEnter() const { return GetTime() - GetEnterTimestamp(); }
    double GetTimeSinceExit() const { return GetTime() - GetExitTimestamp(); }

protected:
    
    void Enter() override
    {
        EnterTimestamp = GetTime(); 
    }
    
    void Exit() override
    {
        ExitTimestamp = GetTime(); 
    }

private:
    
    double EnterTimestamp = 0.0f;
    double ExitTimestamp = 0.0f;
};
