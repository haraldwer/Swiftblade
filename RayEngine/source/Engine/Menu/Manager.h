#pragma once
#include <ranges>

#include "Menu.h"
#include "Debug/Panel.h"

namespace Menu
{
    class Instance;
    
    class Manager : public Utility::Singelton<Manager>, public Debug::Panel
    {
    public:
        
        void Update()
        {
            PROFILE();
            UpdatePending();
            
            Vector<Instance*> ordered;
            for (auto i : std::ranges::reverse_view(stack))
            {
                ordered.push_back(i);
                if (i->IsBlocking())
                    break;
            }

            for (auto i : std::ranges::reverse_view(ordered))
                i->Update();
        }
        
        void Draw() const
        {
            PROFILE();
            CHECK_RETURN(stack.empty());

            Vector<Instance*> ordered;
            for (auto i : std::ranges::reverse_view(stack))
            {
                ordered.push_back(i);
                if (i->IsBlocking())
                    break;
            }

            for (auto i : std::ranges::reverse_view(ordered))
                i->Draw();
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
            if (!stack.empty())
                if (stack.back()->CanBePopped())
                    pending.push_back(stack.back()); 
        }

        void Close(Instance* InPtr)
        {
            if (stackSet.contains(InPtr))
            {
                pending.push_back(InPtr);
                return;
            }
 
            for (int i = 0; i < static_cast<int>(pending.size()); i++)
            {
                if (pending[i] == InPtr)
                {
                    pending.erase(pending.begin() + i);
                    return;
                }
            }
            
            CHECK_ASSERT(true, "Tried to close menu that wasnt open")
        }

        void Clear()
        {
            UpdatePending();
            for (auto i : stack)
                pending.push_back(i);
            UpdatePending(); 
        }

        bool Empty() const { return stack.empty() && pending.empty(); }

        void DrawDebugPanel() override
        {
            int c = 0;
            int count = 0;
            for (auto i : std::ranges::reverse_view(stack))
            {
                if (ImGui::CollapsingHeader(("[" + Utility::ToStr(count) + "] " + i->GetObjName()).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen))
                    i->DebugDraw(c);
                count++;
            }
        }

        String DebugPanelName() const override
        {
            return "Menus";
        }

    private:

        void UpdatePending()
        {
            for (auto p : pending)
            {
                if (stackSet.contains(p))
                { 
                    p->Deinit(); 
                    delete p;
                    stackSet.erase(p);
                    stack.pop_back();
                }
                else
                {
                    stack.push_back(p);
                    stackSet.insert(p);
                    p->Init();
                }
            }
            pending.clear(); 
        }
        
        // Menu stack!
        Set<Instance*> stackSet = {};
        Vector<Instance*> stack = {};
        Vector<Instance*> pending = {};
        
    };
}

