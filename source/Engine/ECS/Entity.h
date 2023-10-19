#pragma once

#include <cstdint>

namespace ECS
{
    // An entity is just an ID
    // And a set of components
    typedef uint32_t ID; 
    constexpr ID InvalidID = -1;
    typedef ID EntityID;
    typedef ID ComponentID; 
}

