#pragma once
#include "Data/Response.h"
#include "Events/Event.h"
#include "Events/Manager.h"
#include "Utility/Singelton.h"

namespace DB
{
    class EventManager : public Utility::Singelton<EventManager, true>, public Utility::EventManager  {};
    
    template <class EventT, class CallbackT = int>
    using Event = Utility::Event<Response<EventT>, EventManager, CallbackT>;
}

