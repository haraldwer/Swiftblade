#pragma once

#include "Database/Data/AuthData.h"

namespace DB
{
    class Authentication
    {
    public:
        void Authenticate(const AuthData& InData);

    private:
        // Load / save auth data
    };
}
