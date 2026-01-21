#pragma once

#include "Utility/Events/Event.h"
#include "Utility/Events/Manager.h"

class InstanceEventManager : public Utility::EventManager, public Utility::Singelton<InstanceEventManager, false> { };
class GlobalEventManager : public Utility::EventManager, public Utility::Singelton<GlobalEventManager, true> { };

template <class EventT, class CallbackT = int>
using InstanceEvent = Utility::Event<EventT, InstanceEventManager, CallbackT>;

template <class EventT, class CallbackT = int>
using GlobalEvent = Utility::Event<EventT, GlobalEventManager, CallbackT>; 