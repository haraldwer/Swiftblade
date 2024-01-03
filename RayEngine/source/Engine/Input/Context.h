#pragma once

#include "Action.h"

namespace Input
{
    struct Context : PropertyOwner<Context>
    {
        PROPERTY(String, Name);
        PROPERTY(Vector<Action>, Actions);
        PROPERTY_D(bool, Blocking, false);
        
        void UpdateCache();
        Map<String, int32> CachedActions;
    };
}