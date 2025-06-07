#pragma once
#include "Elements/Container.h"
#include "Instance.h"

namespace UI
{
    class Builder
    {
    public:

        Builder() = default;
        Builder(const Instance& InInstance, const String& InContainer)
        {
            // Continue building on existing instance...
            instance = InInstance;
            currentContainer = InContainer.empty() ?
                0 : instance.GetID(InContainer); 
        }

        template <class T>
        Builder& Push(const T& InContainer, const String& InIdentifier = String())
        {
            instructions.emplace_back([container = InContainer, id = InIdentifier](Builder& InSelf) {
                InSelf.currentContainer = InSelf.AddInternal(container, id);
            });
            return *this; 
        }

        template <class T>
        Builder& Add(const T& InElement, const String& InIdentifier = String());

        Builder& AddWidget(const Builder& InOther, Transform InTransform = Transform::Fill(), const String& InIdentifier = "")
        {
            // First, push widget instance
            instructions.emplace_back([strID = InIdentifier, trans = InTransform](Builder& InSelf) {
                ElementID id = InSelf.AddInternal<Instance>({}, strID);
                auto& element = InSelf.instance.Get<Instance>(id);
                element.transform = trans;
                InSelf.widgetStack.emplace_back(id);
                InSelf.currentContainer = id;
            });

            // Then copy instructions
            instructions.insert(
                instructions.begin() + instructions.size(),
                InOther.instructions.begin(),
                InOther.instructions.end());

            // Then pop widget instance
            instructions.emplace_back([](Builder& InSelf) {
                const auto last = InSelf.widgetStack.back();
                const auto& lastElem = InSelf.instance.Get<Instance>(last);
                InSelf.currentContainer = lastElem.parent;
                CHECK_ASSERT(InSelf.widgetStack.empty(), "WidgetStack incorrect push/pop")
                InSelf.widgetStack.pop_back();
            });
        
            return *this; 
        }
        
        Builder& Pop()
        {
            instructions.emplace_back([](Builder& InSelf) {
                CHECK_ASSERT(InSelf.currentContainer == 0, "Already at root");
                const auto& elem = InSelf.instance.Get<Instance>(InSelf.currentContainer);
                InSelf.currentContainer = elem.parent;
            });
            return *this;
        }

        Instance Build()
        {
            CHECK_ASSERT(instructions.empty(), "No instructions"); 
            
            // Perform instructions
            for (auto& instruction : instructions)
                instruction(*this);

            // Init
            instance.Invalidate();
            instance.Init();
            
            return instance; 
        }
        
    private:

        template <class T>
        ElementID AddInternal(const T& InElement, const String& InIdentifier)
        {
            // Create element
            const ElementID id = instance.CreateElement<T>(InElement, InIdentifier);
            auto& element = instance.Get<Element>(id);
            element.parent = currentContainer;
            // Add to container
            auto& container = instance.Get<Container>(currentContainer);
            container.children.push_back(id);
            return id; 
        }

        Instance& GetWorkingInstance()
        {
            if (!widgetStack.empty())
                widgetStack.back();
            return instance;
        }

        Instance instance = {};
        ElementID currentContainer = 0;
        Vector<ElementID> widgetStack = {};
        Vector<std::function<void(Builder&)>> instructions = {};
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
