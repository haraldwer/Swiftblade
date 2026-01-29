#pragma once

#include "Events.h"
#include "Instance.h"
#include "Core/Utility/Singelton.h"

namespace Engine
{
    class InstanceManager : public Utility::Singelton<InstanceManager, true>
    {
    public:

        template <class T>
        T* Push()
        {
            const int prevContext = Utility::SingeltonContext::value;
            contextCount++;
            const int newContext = contextCount;
            Utility::SingeltonContext::value = newContext;
            T* ptr = new T();
            queue.push_back({newContext, ptr});
            Utility::SingeltonContext::value = prevContext; 
            return ptr;
        }

        Instance* Top() const
        {
            CHECK_RETURN(stack.empty(), nullptr)
            return stack[stack.size() - 1].ptr;
        }
        
        void Pop()
        {
            queue.emplace_back(); 
        }

        void Tick()
        {
            PROFILE();
            
            for (const auto& entry : queue)
            {
                if (entry.ptr && entry.context != -1)
                {
                    // Add
                    stack.push_back(entry);
                    Utility::SingeltonContext::value = entry.context; 
                    InstanceAddedEvent::Invoke({});
                    entry.ptr->Init();
                }
                else if (!stack.empty())
                {
                    // Remove
                    const auto& top = stack.back();
                    if (top.ptr)
                    {
                        top.ptr->Deinit();
                        delete top.ptr;
                    }
                    stack.pop_back();
                    Utility::SingeltonContext::value = -1;

                    // Set context
                    if (!stack.empty())
                    {
                        const auto& newTop = stack.back();
                        Utility::SingeltonContext::value = newTop.context;
                        InstanceRemovedEvent::Invoke({});
                    }
                }
            }
            queue.clear();
        }
        
        void Clear()
        {
            while (Top())
            {
                Pop();
                Tick(); 
            }
        }

    private:

        struct Entry
        {
            int context = -1;
            Instance* ptr = nullptr; 
        };
        Vector<Entry> queue = {};
        Vector<Entry> stack = {};
        int contextCount = 0; 
        
    };
}
