#pragma once
#include "Config.h"
#include "Events.h"
#include "Components/RPC.h"
#include "Components/Authentication.h"
#include "NakamaTypes.h"
#include "Components/User.h"
#include "Utility/Events/Manager.h"

namespace DB
{
    class Manager : public Utility::Singelton<Manager, true>
    {
        friend class Component;
        friend class Authentication;
        
    public:
        void Init();
        void Tick() const;
        void Deinit();

        EventManager events;
        Authentication auth = {};
        RPC rpc = {};
        User user = {};
        
    private:
        Config config;
        
        Client client = nullptr; 
        RtClient rtClient = nullptr;
        Session session = nullptr;

        Event<AuthResponse>::Callback onAuthenticated = {};
    };
}
