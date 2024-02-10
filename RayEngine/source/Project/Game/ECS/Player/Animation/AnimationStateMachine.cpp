#include "C:/Dev/Swiftblade/RayEngine/build/source/Project/CMakeFiles/Project.dir/Debug/cmake_pch.hxx"
#include "AnimationStateMachine.h"

void AnimationStateMachine::Init()
{
    const auto addState = [&](auto& InState)
    {
        States.emplace_back(reinterpret_cast<StateBase*>(&InState));
    };
    
    addState(Idle); 
    
    StateMachine::Init();
}
