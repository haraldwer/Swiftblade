﻿#pragma once

#include "Action.h"

namespace Input
{
    struct Context : PropertyOwner<Context>
    {
        PROPERTY(String, Name);
        PROPERTY(Vector<Action>, Actions);
        PROPERTY_D(bool, Blocking, false);
        PROPERTY_D(bool, CursorVisible, true);
        
        void UpdateCache();
        Map<String, int32> cachedActions = {};
    };

    struct Config : BaseConfig<Config>
    {
        PROPERTY(Vector<Context>, Contexts);
        
        String Name() const override { return "Input"; }
        bool Edit(const String& InName, uint32 InOffset) override;
        void UpdateCache();
        Map<String, int32> cachedContexts {};
    };
}
