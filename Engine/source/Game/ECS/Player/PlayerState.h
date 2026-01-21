#pragma once

#include "ECS/Player/PlayerInterface.h"
#include "Core/StateMachine/StateBase.h"

class PlayerStateBase : public ECS::PlayerInterface
{
public:
    double GetEnterTimestamp() const { return enterTimestamp; }
    double GetExitTimestamp() const { return enterTimestamp; }
    double GetTimeSinceEnter() const { return GetTime() - GetEnterTimestamp(); }
    double GetTimeSinceExit() const { return GetTime() - GetExitTimestamp(); }
    virtual Type GetAnimationState() const { return Type::None(); } 
    
protected:
    void ApplyAnimationState() const;
    
    double enterTimestamp = 0.0f;
    double exitTimestamp = 0.0f; 
};


template <class T>
class PlayerState : public State<T>, public PlayerStateBase
{
public:
    void Enter() override
    {
        enterTimestamp = GetTime();
        ApplyAnimationState();
    }
    
    void Exit() override
    {
        exitTimestamp = GetTime(); 
    }
};