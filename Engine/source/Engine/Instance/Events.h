#pragma once
#include "Core/Utility/EventScopes.h"

namespace Engine
{
    class Instance;
    
    struct InstanceAddedEventData
    {
    };

    struct InstanceRemovedEventData
    {
    };
}

using InstanceAddedEvent = InstanceEvent<Engine::InstanceAddedEventData>;
using InstanceRemovedEvent = InstanceEvent<Engine::InstanceRemovedEventData>;

template <class T>
using InstanceAddedCallbackT = InstanceAddedEvent::ContextCallback<T>;
using InstanceAddedCallback = InstanceAddedEvent::Callback;

template <class T>
using InstanceRemovedCallbackT = InstanceRemovedEvent::ContextCallback<T>;
using InstanceRemovedCallback = InstanceRemovedEvent::Callback;