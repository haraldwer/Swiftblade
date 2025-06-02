#pragma once
#include "Elements/Container.h"
#include "Instance.h"

namespace UI
{
    class Builder
    {
    public:

        Builder() = default;
        Builder(const ObjectPtr<Instance>& InInstance, const String& InContainer)
        {
            // Continue building on existing instance...
            WorkingInstance = InInstance;
            CurrentContainer = InContainer.empty() ?
                WorkingInstance.Get() : &InInstance->Get<Container>(InContainer); 
        }

        template <class T>
        Builder& Push(const T& InContainer, const String& InIdentifier = String())
        {
            Instructions.emplace_back([container = InContainer, id = InIdentifier](Builder& self) {
                T* elem = self.AddInternal(container, id);
                self.CurrentContainer = elem;
            });
            return *this; 
        }

        template <class T>
        Builder& Add(const T& InElement, const String& InIdentifier = String());

        Builder& AddWidget(const Builder& InBuilder, Transform InTransform = Transform::Fill(), const String& InIdentifier = "")
        {
            // First, push widget instance
            Instructions.emplace_back([id = InIdentifier, trans = InTransform](Builder& self) {
                auto ptr = self.AddInternal<Instance>({}, id);
                ptr->Transform = trans; 
                self.WidgetStack.emplace_back(ptr);
                self.CurrentContainer = ptr;
            });

            // Then copy instructions
            for (auto& i : InBuilder.Instructions)
                Instructions.push_back(i); 

            // Then pop widget instance
            Instructions.emplace_back([](Builder& self) {
                const auto last = self.WidgetStack.back();
                self.CurrentContainer = last->Parent;
                self.WidgetStack.pop_back();
                CHECK_ASSERT(self.WidgetStack.empty(), "WidgetStack incorrect push/pop")
            });
        
            return *this; 
        }
        
        Builder& Pop()
        {
            Instructions.emplace_back([](Builder& self) {
                CHECK_ASSERT(!self.CurrentContainer->Parent, "Already at root");
                self.CurrentContainer = self.CurrentContainer->Parent;
            });
            return *this;
        }

        ObjectPtr<Instance> Build()
        {
            CHECK_ASSERT(Instructions.empty(), "No instructions"); 
            
            // Create instance
            if (!WorkingInstance)
                WorkingInstance = new Instance();
            const ObjectPtr root = WorkingInstance;
            if (!CurrentContainer)
                CurrentContainer = WorkingInstance.Get();
            WidgetStack = { WorkingInstance.Get() };

            // Perform instructions
            for (auto& instruction : Instructions)
                instruction(*this);

            // Init
            root->Invalidate();
            root->Init();
            
            // Reset pointers
            CurrentContainer = nullptr;
            WorkingInstance = nullptr;
            return root; 
        }
        
    private:

        template <class T>
        T* AddInternal(const T& InElement, const String& InIdentifier)
        {
            CHECK_ASSERT(!WorkingInstance, "No working instance");
            CHECK_ASSERT(!CurrentContainer, "No current container");

            // Create element
            T* ptr = new T(InElement);
            ptr->Parent = CurrentContainer;
            CurrentContainer->Elements.push_back(ptr);
            
            if (!InIdentifier.empty())
            {
                CHECK_ASSERT(WidgetStack.empty(), "WidgetStack empty");
                if (const auto entry = WidgetStack.back())
                {
                    CHECK_ASSERT(entry->NamedElements.contains(InIdentifier), "Element with identifier already exists"); 
                    entry->NamedElements[InIdentifier] = ptr;
                }
                
            }
            
            return ptr; 
        }

        Vector<std::function<void(Builder&)>> Instructions = {};
        ObjectPtr<Instance> WorkingInstance = nullptr;
        WeakPtr<Container> CurrentContainer = nullptr;
        Vector<Instance*> WidgetStack = {}; // For naming
    };

    template <>
    Builder& Builder::Add(const Builder& InElement, const String& InIdentifier) = delete; 
    
    template <class T>
    Builder& Builder::Add(const T& InElement, const String& InIdentifier)
    {
        Instructions.emplace_back([element = InElement, id = InIdentifier](Builder& self) {
            self.AddInternal(element, id);
        });
        return *this;
    }
}
