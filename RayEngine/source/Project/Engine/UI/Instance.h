#pragma once
#include "Elements/Container.h"
#include "Engine/EventScopes.h"
#include "Engine/Rendering/RenderEventData.h"

namespace UI
{
    class Instance : public Container
    {
        friend class Builder;
        
    public:
        void Init() override;
        void Invalidate() override;
        
        template <class T = Element>
        T& Get(const String& InIdentifier)
        {
            const auto find = NamedElements.find(InIdentifier);
            CHECK_ASSERT(find == NamedElements.end(), "Entry does not exist")
            const auto ptr = find->second.Get();
            CHECK_ASSERT(!ptr, "Invalid ptr");
            return *reinterpret_cast<T*>(ptr); // TODO: Type safety? 
        }

        template <class T = Element>
        T* TryGet(const String& InIdentifier)
        {
            const auto find = NamedElements.find(InIdentifier);
            CHECK_RETURN(find == NamedElements.end(), nullptr)
            const auto ptr = find->second.Get();
            CHECK_RETURN(!ptr, nullptr);
            return reinterpret_cast<T*>(ptr); // TODO: Type safety? 
        }
        
    private:
        Map<String, WeakPtr<Element>> NamedElements;
        GlobalEvent<OnCreateVirtualTargetData>::Callback OnCreateVirtualTarget;
    };
}
