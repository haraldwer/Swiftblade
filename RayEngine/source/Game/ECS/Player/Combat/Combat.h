﻿#pragma once

#include "ECS/Player/PlayerInterface.h"
#include "Engine/ECS/UniqueComponent.h"

class CombatStateMachine;

namespace ECS
{
    class Combat : public UniqueComponent<Combat>, public PlayerInterface
    {
    public:
        void Init() override;
        void Update() override;
        bool EditState() const;

    private:
        ObjectPtr<CombatStateMachine> StateMachine;
    };
}
