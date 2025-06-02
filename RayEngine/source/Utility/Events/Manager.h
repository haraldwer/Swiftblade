#pragma once

namespace Utility
{
    class BaseCallback;

    // Track all the callbacks
    // Bind to corresponding event
    class EventManager
    {
    public:
        void Register(const Type& InEventType, BaseCallback* InCallback)
        {
            callbacks[InEventType.GetHash()].insert(InCallback); 
        }
        
        void Unregister(const Type& InEventType, BaseCallback* InCallback)
        {
            callbacks[InEventType.GetHash()].erase(InCallback); 
        }

        const Set<BaseCallback*>& GetCallbacks(const Type& InEventType)
        {
            return callbacks[InEventType.GetHash()];
        }
        
    private:
        Map<TypeHash, Set<BaseCallback*>> callbacks; 
    };
}

