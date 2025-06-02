#pragma once

#include "Instance.h"
#include "Core/Utility/Singelton.h"

namespace Engine
{
    class Manager : public Utility::Singelton<Manager, true>
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
            Queue.push_back({newContext, ptr});
            Utility::SingeltonContext::value = prevContext; 
            return ptr;
        }

        Instance* Top() const
        {
            CHECK_RETURN(Stack.empty(), nullptr)
            return Stack[Stack.size() - 1].Ptr;
        }
        
        void Pop()
        {
            Queue.emplace_back(); 
        }

        void Update()
        {
            PROFILE();
            
            for (const auto& entry : Queue)
            {
                if (entry.Ptr && entry.Context != -1)
                {
                    // Add
                    Stack.push_back(entry);
                    Utility::SingeltonContext::value = entry.Context; 
                    entry.Ptr->Init();
                }
                else if (!Stack.empty())
                {
                    // Remove
                    const auto& top = Stack.back();
                    if (top.Ptr)
                    {
                        top.Ptr->Deinit();
                        delete top.Ptr;
                    }
                    Stack.pop_back();
                    Utility::SingeltonContext::value = -1;

                    // Set context
                    if (!Stack.empty())
                    {
                        const auto& newTop = Stack.back();
                        Utility::SingeltonContext::value = newTop.Context;
                    }
                }
            }
            Queue.clear();
        }
        
        void Clear()
        {
            while (Top())
            {
                Pop();
                Update(); 
            }
        }

    private:

        struct Entry
        {
            int Context = -1;
            Instance* Ptr = nullptr; 
        };
        Vector<Entry> Queue = {};
        Vector<Entry> Stack = {};
        int contextCount = 0; 
        
    };
}
