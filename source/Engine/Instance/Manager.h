#pragma once

#include "Instance.h"
#include "Utility/Singelton.h"

namespace Engine
{
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:

        template <class T>
        T* Push()
        {
            Utility::SingeltonContext::Value++;
            T* ptr = new T();
            Stack.push_back(ptr);
            ptr->Init(); 
            return ptr;
        }

        InstanceBase* Top() const
        {
            CHECK_RETURN(Stack.empty(), nullptr)
            return Stack[Stack.size() - 1];
        }
        
        void Pop()
        {
            auto* top = Top();
            CHECK_RETURN(!top); 
            top->Deinit();
            delete top;
            Stack.pop_back();
            Utility::SingeltonContext::Value--; 
        }
        void Clear()
        {
            while (Top())
                Pop();
        }

    private:
        
        Vector<InstanceBase*> Stack;
        
    };
}