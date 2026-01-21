#pragma once

namespace Debug
{
    struct Config : BaseConfig<Config>
    {
        PROPERTY(Set<String>, OpenWindows);
        PROPERTY_D(bool, DebugEnabled, true);
        String Name() const override { return "Debug"; };
    };
}
