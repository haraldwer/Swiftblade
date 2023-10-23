#pragma once

namespace ECS
{
    // An entity is just an ID
    // And a set of components
    typedef uint32 ID; 
    constexpr ID InvalidID = -1;
    typedef ID EntityID;
    typedef ID ComponentID; 
}

