#include "Combat.h"

#include "CombatStateMachine.h"

void ECS::Combat::Init()
{
    StateMachine = new CombatStateMachine();
    StateMachine->Init();
}

void ECS::Combat::Update()
{
    if (StateMachine)
        StateMachine->Update();
}

bool ECS::Combat::Edit(const String& InName)
{
    if (StateMachine)
        StateMachine->Edit();
    return UniqueComponent::Edit(InName);
}
