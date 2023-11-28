#pragma once
#include "Elements/Container.h"
#include "Instance.h"

namespace UI
{
    class Builder
    {
    public:

        Builder() = default;

        template <class T>
        Builder& Push(const T& InContainer)
        {
            T* elem = AddInternal(InContainer);

            // No root, set container as root
            if (!CurrentContainer)
            {
                CHECK_ASSERT(WorkingInstance.Root, "Root already set")
                WorkingInstance = Instance();
                WorkingInstance.Root = elem;
            }
            CurrentContainer = elem; 
            return *this; 
        }

        template <class T>
        Builder& Add(const T& InElement)
        {
            CHECK_ASSERT(!CurrentContainer, "Current not set")
            AddInternal(InElement);
            return *this; 
        }
        
        Builder& Pop()
        {
            CHECK_ASSERT(!CurrentContainer->Parent, "Already at root");
            CurrentContainer = CurrentContainer->Parent;
            return *this;
        }

        Instance Build()
        {
            CurrentContainer = nullptr;
            WorkingInstance.Init();
            const Instance instance = WorkingInstance;
            WorkingInstance = Instance(); 
            return instance; 
        }
        
    private:

        template <class T>
        T* AddInternal(const T& InElement)
        {
            auto ptr = new T(InElement);
            if (CurrentContainer)
            {
                ptr->Parent = CurrentContainer;
                CurrentContainer->Elements.push_back(ptr);
            }
            return ptr; 
        }
        
        Instance WorkingInstance;
        ObjectPtr<Container> CurrentContainer;
    };
}
