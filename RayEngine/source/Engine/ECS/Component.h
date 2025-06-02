#pragma once

#include "Entity.h"

namespace ECS
{
    class ComponentBase
    {
    public:
        virtual ~ComponentBase() = default;
        void SetID(const EntityID InID) { id = InID; }
        EntityID GetID() const
        {
            CHECK_ASSERT(id == InvalidID, "Invalid ID");
            return id;
        }
    private: 
        EntityID id = INVALID_ID;
    };
    
    template <class T>
    class Component : public PropertyOwner<T>, public ComponentBase
    {
    };
}
