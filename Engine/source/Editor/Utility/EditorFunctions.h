#pragma once

#include "ECS/Entity.h"

namespace Editor
{
    bool EditHierarchy(ECS::EntityID InRootID, ECS::EntityID& InOutSelected, bool InRecursive = true);
    bool EditScene(ECS::EntityID& InOutSelected);
    bool EditComponents(ECS::EntityID InID);
}
