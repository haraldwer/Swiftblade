#pragma once

#include "Elements/Container.h"
#include "Rendering/Events.h"

namespace UI
{
    class Instance : public Container
    {
        friend class Builder;
        
    public:
        void Init();
        void Invalidate();
        void Draw();
        

        template <class T>
        T& Get(const ElementID InID)
        {
            auto find = elements.find(InID);
            CHECK_ASSERT(find == elements.end(), "Failed to find element");
            return find->second.Get<T>();
        }

        Instance& Get(const ElementID InID)
        {
            if (InID == 0)
                return *this;
            auto find = elements.find(InID);
            CHECK_ASSERT(find == elements.end(), "Failed to find element");
            return find->second.Get<Instance>();
        }

        ElementID GetID(const String& InName)
        {
            auto find = namedElements.find(InName);
            CHECK_ASSERT(find == namedElements.end(), "Failed to find element");
            return find->second;
        }
        
        template <class T>
        T& Get(const String& InName)
        {
            return Get<T>(InName);
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
            elements[idCounter] = Object<T>(InElement);
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
