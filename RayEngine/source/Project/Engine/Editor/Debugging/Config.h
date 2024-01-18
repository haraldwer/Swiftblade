#pragma once

#include "Engine/BaseConfig.h"

namespace Debug
{
    struct Config : BaseConfig<Config>
    {
        PROPERTY(Set<String>, OpenWindows);
        String Name() const override { return "Debug"; };
    };
}
