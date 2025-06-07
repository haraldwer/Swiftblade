#pragma once

#include "Elements/Container.h"
#include "Rendering/Events.h"

namespace UI
{
    class Instance : public Container
    {
        friend class Builder;

        TYPE_INFO(Instance, Container)
        
    public:
        void Init();
        void Invalidate();
        void Update();
        void Draw();

        template <class T>
        T& Get(const ElementID InID)
        {
            if (InID == 0 && IsA(Type::Get<T>()))
                return *reinterpret_cast<T*>(this);
            auto find = elements.find(InID);
            CHECK_ASSERT(find == elements.end(), "Failed to find element");
            return find->second.Get<T>();
        }

        template <class T>
        const T& Get(const ElementID InID) const
        {
            if (InID == 0 && IsA(Type::Get<T>()))
                return *reinterpret_cast<const T*>(this);
            auto find = elements.find(InID);
            CHECK_ASSERT(find == elements.end(), "Failed to find element");
            return find->second.Get<T>();
        }

        ElementID GetID(const String& InName) const
        {
            auto find = namedElements.find(InName);
            CHECK_ASSERT(find == namedElements.end(), "Failed to find element");
            return find->second;
        }
        
        template <class T>
        T& Get(const String& InName)
        {
            return Get<T>(GetID(InName));
        }

        template <class T>
        const T& Get(const String& InName) const
        {
            return Get<T>(GetID(InName));
        }

        Instance& operator [](const String& InName)
        {
            return Get<Instance>(InName);
        }

        const Instance& operator [](const String& InName) const
        {
            return Get<Instance>(InName);
        }

        template <class T>
        T* TryGet(const String& InName)
        {
            auto find = namedElements.find(InName);
            CHECK_RETURN(find == namedElements.end(), nullptr)
            return &Get<T>(find->second);
        }
    
    private:

        // Hide access
        void Init(Instance &InInstance) override;
        void Invalidate(Instance &InInstance) override;
        void Draw(Instance &InInstance) override;

        template <class T>
        ElementID CreateElement(const T& InElement, const String& InIdentifier)
        {
            idCounter++;
            elements[idCounter] = InElement;
            if (!InIdentifier.empty())
                namedElements[InIdentifier] = idCounter;
            return idCounter;
        }
        
        ElementID idCounter = 0;
        Map<ElementID, Object<Element>> elements = {};
        Map<String, ElementID> namedElements = {};
        
        GlobalEvent<OnSetViewportSize>::Callback onSetViewportSize = {};
    };
}
