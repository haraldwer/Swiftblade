#pragma once

namespace DB
{
    struct Config : BaseConfig<Config>
    {
        PROPERTY(String, ServerKey);
        PROPERTY(String, Address);
        PROPERTY_D(int, Port, -1);
        PROPERTY_D(bool, SSL, false);

        String Name() const override { return "Database"; }
    };
}
