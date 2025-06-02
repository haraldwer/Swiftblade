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
        void Update();
        void Deinit();

        Authentication Auth = {};
        Leaderboard LB = {};
        Blob Blob = {};
        
    private:
        Client Client = nullptr; 
        RtClient RtClient = nullptr;
        Session Session = nullptr;

        Utility::Event<OnLoginSuccess, Manager, int>::Callback OnLoggedIn = {};
    };

    template <class EventT, class CallbackT = int>
    using DBEvent = Utility::Event<EventT, Manager, CallbackT>;
}
