#pragma once

#include "Component.h"
#include "Database/Data/AuthData.h"

namespace DB
{
    class Authentication : public Component 
    {
    public:
        void Authenticate(const AuthData& InData) const;

        bool IsAuthenticated() const;
        String GetUsername();

    private:

        void OnSuccess() const;
        void OnFailed(const Nakama::NError& InError) const;
        
        // Load / save auth data
    };
}
