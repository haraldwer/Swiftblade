﻿#pragma once
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
            
            CHECK_RETURN(stack.empty());
            for (int i = static_cast<int>(stack.size()) - 1; i >= 0; i--)
            {
                const auto ptr = stack[i].Get();
                CHECK_CONTINUE(!ptr);
                ptr->Update();
                if (ptr->IsBlocking())
                    break; 
            }
        }
        
        void Draw() const
        {
            PROFILE();
            CHECK_RETURN(stack.empty());
            for (int i = static_cast<int>(stack.size()) - 1; i >= 0; i--)
            {
                const auto ptr = stack[i].Get();
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
            pending.push_back(reinterpret_cast<Instance*>(ptr));
            return ptr;
        }

        void Pop()
        {
            pending.emplace_back(nullptr); 
        }

        void Clear()
        {
            for (auto s : stack)
                Pop();
            UpdatePending(); 
        }

    private:

        void UpdatePending()
        {
            for (auto p : pending)
            {
                if (p)
                {
                    stack.push_back(p);
                    p->Init();
                }
                else
                {
                    if (Instance* ptr = stack.back().Get())
                        ptr->Deinit(); 
                    stack.pop_back(); 
                }
            }
            pending.clear(); 
        }
        
        // Menu stack!
        Vector<ObjectPtr<Instance>> stack = {};
        Vector<ObjectPtr<Instance>> pending = {};
        
    };
}

