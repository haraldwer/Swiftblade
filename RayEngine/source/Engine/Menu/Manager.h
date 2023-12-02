#pragma once
#include "Menu.h"

namespace Menu
{
    class Instance;
    
    class Manager : public Utility::Singelton<Manager>
    {
    public:
        
        void Update(double InDelta) const
        {
            CHECK_RETURN(Stack.empty());
            const auto copy = Stack;
            for (int i = static_cast<int>(copy.size()) - 1; i >= 0; i--)
            {
                const auto ptr = copy[i].Get();
                CHECK_CONTINUE(!ptr);
                ptr->Update(InDelta);
                if (ptr->IsBlocking())
                    break; 
            }
        }
        
        void Draw() const
        {
            CHECK_RETURN(Stack.empty());
            const auto copy = Stack; 
            for (int i = static_cast<int>(copy.size()) - 1; i >= 0; i--)
            {
                const auto ptr = copy[i].Get();
                CHECK_CONTINUE(!ptr);
                ptr->Draw();
                if (ptr->IsBlocking())
                    break; 
            }
        }

        template <class T>
        T* Push()
        {
            T* ptr = new T;
            Stack.push_back(reinterpret_cast<Instance*>(ptr));
            ptr->Init(); 
            return ptr;
        }

        void Pop()
        {
            if (const auto ptr = Stack.back().Get())
                ptr->Deinit(); 
            Stack.pop_back(); 
        }

        void Clear()
        {
            while (!Stack.empty())
                Pop(); 
        }

    private:
        
        // Menu stack!
        Vector<ObjectPtr<Instance>> Stack; 
        
    };
}

