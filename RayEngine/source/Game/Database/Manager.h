#pragma once
#include "Events.h"
#include "Components/RPC.h"
#include "Components/Authentication.h"
#include "NakamaTypes.h"
#include "Components/User.h"
#include "Utility/Events/Manager.h"

namespace DB
{
    class Manager : public Utility::Singelton<Manager>
    {
        friend class Component;
        friend class Authentication;
        
    public:
        void Init();
        void Update() const;
        void Deinit();

        EventManager events;
        Authentication auth = {};
        RPC rpc = {};
        User user = {};
        
    private:
        Client client = nullptr; 
        RtClient rtClient = nullptr;
        Session session = nullptr;

        Event<AuthResponse>::Callback onAuthenticated = {};
    };
}
