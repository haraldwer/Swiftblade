#pragma once

#include "Manager.h"
#include "UniqueComponent.h"

#define ECS_REG_ENTITY_SYSTEM(x) \
ECS::Manager::Get().RegisterSystem(new CONCAT(Sys, x), String(#x), Utility::Type::GetHash<x>(), Utility::Type::GetHash<CONCAT(Sys, x)>())

#define ECS_REG_UNIQUE_COMPONENT(x) \
ECS::Manager::Get().RegisterSystem(new UniqueSystem<x>, String(#x), Utility::Type::GetHash<x>(), Utility::Type::GetHash<UniqueSystem<x>>())

namespace ECS
{
    void RegisterEngineSystems();
}