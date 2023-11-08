#pragma once

#include "Engine/Property/PropertyOwner.h"
#include "Engine/Property/Property.h"

namespace ECS
{
    class ComponentBase
    {
    };
    
    template <class T>
    class Component : public PropertyOwner<T>, public ComponentBase
    {
    };
}
