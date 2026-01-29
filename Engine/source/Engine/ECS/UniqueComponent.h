#pragma once

#include "Component.h"
#include "Manager.h"
#include "System.h"

namespace ECS
{
    // For components that there are few of,
    // which do not benefit from ECS structure.
    // Provides a nice interface. 
    // Note: can still be moved in memory
    class UniqueComponentBase : public ComponentBase
    {
    public:
        
        virtual void Init() {}
        virtual void Deinit() {}
        virtual void Tick() {}
        virtual int GetPriority() const { return 0; }
        virtual bool ShouldTick() const;

    protected:

        template <class T>
        T& Get() const
        {
            T* comp = TryGet<T>(GetID());
            CHECK_ASSERT(!comp, "Invalid component");
            return *comp;
        }
        
        template <class T>
        T& Get(const EntityID InID) const
        {
            T* comp = TryGet<T>(InID);
            CHECK_ASSERT(!comp, "Invalid component");
            return *comp;
        }

        template <class T>
        static T* TryGet(const EntityID InID)
        {
            return Manager::Get().GetComponent<T>(InID);
        }

        template <class T>
        static T& GetSystem()
        {
            return Manager::Get().GetSystem<T>(); 
        }
    };
    
    template <class T>
    class UniqueComponent : public PropertyOwner<T>, public UniqueComponentBase
    {
    };

    template <class T>
    class UniqueSystem : public System<T>
    {
        void Init(EntityID InID, T& InComponent) override
        {
            InComponent.Init();
        }

        void Deinit(EntityID InID, T& InComponent) override
        {
            InComponent.Deinit(); 
        }

        void Tick(EntityID InID, T& InComponent) override
        {
            InComponent.Tick(); 
        }

        int GetPriority() const override
        {
            return T().GetPriority();
        }

        bool ShouldTick() const override
        {
            return T().ShouldTick();
        }
    };
}
