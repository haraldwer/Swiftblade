#pragma once

#include <functional>

/*
// Example usage:
void func()
{
    // Create a struct containing event data
    // This struct is used as identifier
    struct MyEventData
    {
        int Value;
    };

    // Set up callback and bind functions
    Event<MyEventData>::Callback callback;
    callback.Bind([](const MyEventData& InParameter)
    {
        LOG(std::to_string(InParameter.Value));
    });

    // Invoke the event, pass the data
    Event<MyEventData> event;
    event.Invoke({ 1 });
}
*/

namespace Utility
{
    // Shared base class for callbacks
    class BaseCallback { };

    template <class EventT, class EventManagerT, class CallbackT>
    class Event
    {
    public:
        virtual ~Event() = default;

        // Internal callback class
        class Callback : public BaseCallback
        {
            friend Event;
            
        public:
            Callback()
            {
                EventManagerT::Get().Register(Utility::GetType<Event>(), this);
            }
            
            ~Callback()
            {
                EventManagerT::Get().Unregister(Utility::GetType<Event>(), this);
            }
            
            void Bind(const std::function<void(const EventT&)>& InFunc, const CallbackT& InCallbackData = {})
            {
                Bindings.push_back({ InFunc, InCallbackData }); 
            }

        private:
            struct Binding
            {
                std::function<void(const EventT&)> Function;
                CallbackT CallbackData;
            };
            
            Vector<Binding> Bindings; 
        };
        
        void Invoke(const EventT& InData)
        {
            for (auto callback : EventManagerT::Get().GetCallbacks(Utility::GetType<Event>()))
            {
                auto* callbackPtr = reinterpret_cast<Callback*>(callback); 
                for (const auto& binding : callbackPtr->Bindings)
                    if (ShouldInvoke(InData, binding.CallbackData))
                        binding.Function(InData);
            } 
        }

        // Event implementations can override this to limit invoked bindings
        // For example, an entity event that should only occur on specific entities
        virtual bool ShouldInvoke(const EventT& InEventData, const CallbackT& InCallbackData)
        {
            return true; 
        }
    };
}
