#pragma once

#include "Elements/Container.h"

namespace UI
{
    class Builder
    {
    public:

        Builder() = default;
        Builder(const Container& InInstance)
        {
            instance = InInstance;
        }

        void Push(const String& InIdentifier)
        {
            auto& i = GetInstance();
            auto find = i.namedElements.find(InIdentifier);
            CHECK_ASSERT(find == i.namedElements.end(), "No such container");
            stack.push_back(find->second);
        }
        
        template <class T>
        Builder& Push(const T& InContainer, const String& InIdentifier = String())
        {
            CHECK_ASSERT(!T::IsA(Type::Get<Container>()), "Can only push containers");
            stack.push_back(GetInstance().Add<T>(InContainer, InIdentifier));
            return *this; 
        }

        template <class T>
        Builder& Add(const T& InElement, const String& InIdentifier = String());

        Builder& Pop()
        {
            CHECK_ASSERT(stack.empty(), "Already at root");
            stack.pop_back();
            return *this;
        }

        Container Build()
        {
            stack.clear();
            instance.Invalidate();
            instance.Init();
            return instance; 
        }
        
    private:
        
        Container& GetInstance()
        {
            Container* i = &instance;
            for (auto& id : stack)
                i = &i->Get<Container>(id);
            CHECK_ASSERT(!i, "Invalid instance");
            return *i;
        }
        
        Container instance = {};
        Vector<ElementID> stack = {}; 
    };

    template <>
    Builder& Builder::Add(const Builder& InElement, const String& InIdentifier) = delete; 
    
    template <class T>
    Builder& Builder::Add(const T& InElement, const String& InIdentifier)
    {
        GetInstance().Add<T>(InElement, InIdentifier);
        return *this;
    }
}
