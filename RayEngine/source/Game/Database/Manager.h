#pragma once
#include "Components/Authentication.h"
#include "Components/Blob.h"
#include "Components/Leaderboard.h"
#include "NakamaTypes.h"
#include "Utility/Events/Event.h"
#include "Utility/Events/Manager.h"

namespace DB
{
    class Manager : public Utility::Singelton<Manager>, public Utility::EventManager
    {
        friend class Component;
        friend class Authentication;
        
    public:
        void Init();
        void Update() const;
        void Deinit();

        Authentication auth = {};
        Leaderboard lb = {};
        Blob blob = {};
        
    private:
        Client client = nullptr; 
        RtClient rtClient = nullptr;
        Session session = nullptr;

        Utility::Event<OnLoginSuccess, Manager, int>::Callback onLoggedIn = {};
    };

    template <class EventT, class CallbackT = int>
    using DBEvent = Utility::Event<EventT, Manager, CallbackT>;
}
