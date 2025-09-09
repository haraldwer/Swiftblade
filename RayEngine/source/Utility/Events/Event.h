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
    Event<MyEventData>::Invoke({ 1 });
}
*/

namespace Utility
{
    // Shared base class for callbacks
    class BaseCallback
    {
    public:
        virtual ~BaseCallback() = default;
    };

    template <class EventT, class EventManagerT, class CallbackDataT = int>
    class Event
    {
        // Event specific callback
        class EventCallback : public BaseCallback
        {
            friend Event;
        public:
            ~EventCallback() override = default;
        private:
            virtual void Invoke(const EventT& InData) const {}
        };
        
    public:
        virtual ~Event() = default;
        
        // Internal callback class
        template <class ContextT>
        class ContextCallback : public EventCallback
        {
        public:
            ContextCallback()
            {
                EventManagerT::Get().Register(Type::Get<Event>(), this);
            }

            ~ContextCallback() override
            {
                EventManagerT::Get().Unregister(Type::Get<Event>(), this);
            }

            ContextCallback(const ContextCallback& InOther)
            {
                EventManagerT::Get().Register(Type::Get<Event>(), this);
                bindings = InOther.bindings;
                context = InOther.context;
            }

            void SetContext(const ContextT& InC)
            {
                context = InC;
            }
            
            void Bind(const std::function<void(const EventT&, const ContextT& InContext)>& InFunc, const CallbackDataT& InCallbackData = {})
            {
                bindings.push_back({ InFunc, InCallbackData }); 
            }

        private:

            void Invoke(const EventT& InData) const override
            {
                for (const auto& binding : bindings)
                    if (ShouldInvoke(InData, context, binding.callbackData))
                        binding.function(InData, context);
            }
            
            // Event implementations can override this to limit invoked bindings
            // For example, an entity event that should only occur on specific entities
            virtual bool ShouldInvoke(const EventT& InEventData, const ContextT InContext, const CallbackDataT& InCallbackData) const
            {
                return true; 
            }
            
            struct Binding
            {
                std::function<void(const EventT&, const ContextT& InContext)> function;
                CallbackDataT callbackData;
            };
            
            Vector<Binding> bindings;
            ContextT context;
        };

        typedef ContextCallback<int> Callback;

        static void Invoke(const EventT& InData)
        {
            for (auto callback : EventManagerT::Get().GetCallbacks(Type::Get<Event>()))
                if (auto* callbackPtr = reinterpret_cast<EventCallback*>(callback))
                    callbackPtr->Invoke(InData);
        }
    };
}
