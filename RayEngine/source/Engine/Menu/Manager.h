#pragma once
#include "Menu.h"

namespace Menu
{
    class Instance;
    
    class Manager : public Utility::Singelton<Manager>
    {
    public:
        
        void Update()
        {
            PROFILE();
            UpdatePending();
            
            CHECK_RETURN(Stack.empty());
            for (int i = static_cast<int>(Stack.size()) - 1; i >= 0; i--)
            {
                const auto ptr = Stack[i].Get();
                CHECK_CONTINUE(!ptr);
                ptr->Update();
                if (ptr->IsBlocking())
                    break; 
            }
        }
        
        void Draw() const
        {
            PROFILE();
            CHECK_RETURN(Stack.empty());
            for (int i = static_cast<int>(Stack.size()) - 1; i >= 0; i--)
            {
                const auto ptr = Stack[i].Get();
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
            Pending.push_back(reinterpret_cast<Instance*>(ptr));
            return ptr;
        }

        void Pop()
        {
            Pending.emplace_back(nullptr); 
        }

        void Clear()
        {
            for (auto s : Stack)
                Pop();
            UpdatePending(); 
        }

    private:

        void UpdatePending()
        {
            for (auto p : Pending)
            {
                if (p)
                {
                    Stack.push_back(p);
                    p->Init();
                }
                else
                {
                    if (Instance* ptr = Stack.back().Get())
                        ptr->Deinit(); 
                    Stack.pop_back(); 
                }
            }
            Pending.clear(); 
        }
        
        // Menu stack!
        Vector<ObjectPtr<Instance>> Stack = {};
        Vector<ObjectPtr<Instance>> Pending = {};
        
    };
}

