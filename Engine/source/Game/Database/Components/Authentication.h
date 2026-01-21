#pragma once

#include "Component.h"
#include "Database/Events.h"
#include "Database/Data/Auth.h"

namespace DB
{
    class Authentication : public Component 
    {
    public:
        void Init(Manager *InManager) override;
        void Deinit() override;

        void Authenticate();
        void AuthenticateDevice();
        void AuthenticateSteam();
        void LinkSteam() const;
        bool IsAuthenticated() const;
    
    private:
        Event<AuthResponse>::Callback onAuthenticated;
        Event<DeviceAuthResponse>::Callback onDevice;
        Event<SteamAuthResponse>::Callback onSteam;
        bool authenticated = false;
    };
}
