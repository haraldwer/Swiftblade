#pragma once

#include "Entity.h"

namespace ECS
{
    class ComponentBase
    {
    public:
        virtual ~ComponentBase() = default;
        void SetID(const EntityID InID) { ID = InID; }
        EntityID GetID() const
        {
            CHECK_ASSERT(ID == InvalidID, "Invalid ID");
            return ID;
        }
    private: 
        EntityID ID = InvalidID;
    };
    
    template <class T>
    class Component : public PropertyOwner<T>, public ComponentBase
    {
    };
}
