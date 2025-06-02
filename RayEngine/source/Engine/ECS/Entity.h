#pragma once

namespace ECS
{
    // An entity is just an ID
    // And a set of components
    typedef uint32 ID; 
    constexpr ID INVALID_ID = -1;
    typedef ID EntityID;
    typedef ID ComponentID;

    inline void* EntityToPtr(const EntityID InID)
    {
        const uint64 longID = static_cast<uint64>(InID);
        return reinterpret_cast<void*>(longID); 
    }

    inline EntityID PtrToEntity(const void* InPtr)
    {
        const uint64 longID = reinterpret_cast<uint64>(InPtr);
        return static_cast<EntityID>(longID);  
    }
}

