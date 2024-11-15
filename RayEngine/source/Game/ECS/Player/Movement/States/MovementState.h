#pragma once

#include "ECS/Player/PlayerInterface.h"
#include "Engine/Utility/StateMachine/StateBase.h"

namespace Physics
{
    struct Contact;
}

class MovementStateBase : public ECS::PlayerInterface
{
public:
    double GetEnterTimestamp() const { return EnterTimestamp; }
    double GetExitTimestamp() const { return EnterTimestamp; }
    double GetTimeSinceEnter() const { return GetTime() - GetEnterTimestamp(); }
    double GetTimeSinceExit() const { return GetTime() - GetExitTimestamp(); }
    virtual Type GetAnimationState() const { return Type::None(); } 
    
protected:
    void ApplyAnimationState() const;
    
    double EnterTimestamp = 0.0f;
    double ExitTimestamp = 0.0f; 
};

template <class T>
class MovementState : public State<T>, public PropertyOwner<T>, public MovementStateBase
{
protected:
    void Enter() override
    {
        EnterTimestamp = GetTime();
        ApplyAnimationState();
    }
    
    void Exit() override
    {
        ExitTimestamp = GetTime(); 
    }
};
