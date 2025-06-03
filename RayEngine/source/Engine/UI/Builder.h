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
            workingInstance = InInstance;
            currentContainer = InContainer.empty() ?
                workingInstance.Get() : &InInstance->Get<Container>(InContainer); 
        }

        template <class T>
        Builder& Push(const T& InContainer, const String& InIdentifier = String())
        {
            instructions.emplace_back([container = InContainer, id = InIdentifier](Builder& InSelf) {
                T* elem = InSelf.AddInternal(container, id);
                InSelf.currentContainer = elem;
            });
            return *this; 
        }

        template <class T>
        Builder& Add(const T& InElement, const String& InIdentifier = String());

        Builder& AddWidget(const Builder& InBuilder, Transform InTransform = Transform::Fill(), const String& InIdentifier = "")
        {
            // First, push widget instance
            instructions.emplace_back([id = InIdentifier, trans = InTransform](Builder& InSelf) {
                auto ptr = InSelf.AddInternal<Instance>({}, id);
                ptr->transform = trans; 
                InSelf.widgetStack.emplace_back(ptr);
                InSelf.currentContainer = ptr;
            });

            // Then copy instructions
            for (auto& i : InBuilder.instructions)
                instructions.push_back(i); 

            // Then pop widget instance
            instructions.emplace_back([](Builder& InSelf) {
                const auto last = InSelf.widgetStack.back();
                InSelf.currentContainer = last->parent;
                InSelf.widgetStack.pop_back();
                CHECK_ASSERT(InSelf.widgetStack.empty(), "WidgetStack incorrect push/pop")
            });
        
            return *this; 
        }
        
        Builder& Pop()
        {
            instructions.emplace_back([](Builder& InSelf) {
                CHECK_ASSERT(!InSelf.currentContainer->parent, "Already at root");
                InSelf.currentContainer = InSelf.currentContainer->parent;
            });
            return *this;
        }

        ObjectPtr<Instance> Build()
        {
            CHECK_ASSERT(instructions.empty(), "No instructions"); 
            
            // Create instance
            if (!workingInstance)
                workingInstance = new Instance();
            const ObjectPtr root = workingInstance;
            if (!currentContainer)
                currentContainer = workingInstance.Get();
            widgetStack = { workingInstance.Get() };

            // Perform instructions
            for (auto& instruction : instructions)
                instruction(*this);

            // Init
            root->Invalidate();
            root->Init();
            
            // Reset pointers
            currentContainer = nullptr;
            workingInstance = nullptr;
            return root; 
        }
        
    private:

        template <class T>
        T* AddInternal(const T& InElement, const String& InIdentifier)
        {
            CHECK_ASSERT(!workingInstance, "No working instance");
            CHECK_ASSERT(!currentContainer, "No current container");

            // Create element
            T* ptr = new T(InElement);
            ptr->Parent = currentContainer;
            currentContainer->elements.push_back(ptr);
            
            if (!InIdentifier.empty())
            {
                CHECK_ASSERT(widgetStack.empty(), "WidgetStack empty");
                if (const auto entry = widgetStack.back())
                {
                    CHECK_ASSERT(entry->namedElements.contains(InIdentifier), "Element with identifier already exists"); 
                    entry->namedElements[InIdentifier] = ptr;
                }
                
            }
            
            return ptr; 
        }

        Vector<std::function<void(Builder&)>> instructions = {};
        ObjectPtr<Instance> workingInstance = nullptr;
        WeakPtr<Container> currentContainer = nullptr;
        Vector<Instance*> widgetStack = {}; // For naming
    };

    template <>
    Builder& Builder::Add(const Builder& InElement, const String& InIdentifier) = delete; 
    
    template <class T>
    Builder& Builder::Add(const T& InElement, const String& InIdentifier)
    {
        instructions.emplace_back([element = InElement, id = InIdentifier](Builder& InSelf) {
            InSelf.AddInternal(element, id);
        });
        return *this;
    }
}
