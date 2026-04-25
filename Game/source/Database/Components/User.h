#pragma once
#include "Component.h"
#include "Database/Events.h"
#include "Database/Data/RPCUser.h"

namespace DB
{
    class User : public Component
    {
    public:
        void Init(Manager *InManager) override;
        void Deinit() override;

        void TryLogin(const String& InName = "") const;
        
        bool IsLoggedIn() const;
        String GetUsername() const { return username; }
        
    private:
        bool loggedIn = false; 
        String username;

        Event<RPCLogin>::Callback onLogin;
    };
}
