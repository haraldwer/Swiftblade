#pragma once

#include "Entity.h"

namespace Physics
{
    struct Contact;
}

namespace ECS
{
    class Manager;

    class SystemBase
    {
        friend Manager; 
        
    public:
        virtual ~SystemBase() = default;

        // Implemented by EntitySystem<T>

        virtual ComponentID Register(EntityID InID, bool InDeferredInit = false) = 0;
        virtual void FinishRegistration(EntityID InID) = 0;
        virtual void Unregister(EntityID InID) = 0;
        virtual void Serialize(EntityID InID, SerializeObj& OutObj) = 0; 
        virtual bool Deserialize(EntityID InID, const DeserializeObj& InObj) = 0; 
        virtual bool Edit(EntityID InID) = 0; 
        
        // Can be overriden by T
        
        virtual void SystemInit() = 0;
        virtual void SystemTick() = 0;
        virtual void SystemFrame() = 0;
        virtual bool ShouldTick() const;
        virtual int GetPriority() const { return 0; }

        static SystemBase* GetAnonymousSystem(const Utility::Type& InType, bool InIsCompHash);
        bool Contains(EntityID InID) const;
        Vector<EntityID> GetEntities() const;
        bool Count() const { return entityToComponent.size(); }
        bool Empty() const { return entityToComponent.empty(); }

    protected:

        ComponentID Translate(EntityID InID) const;

        ComponentID RegisterID(EntityID InID);
        void UnregisterID(EntityID InID, ComponentID InCompID);
        virtual ComponentID NewID() = 0;

        // For memory aligned iteration
        const OrderedMap<ComponentID, EntityID>& ComponentMap() { return componentToEntity; }
        const Map<EntityID, ComponentID>& EntityMap() { return entityToComponent; }
        
    private:
        
        Map<EntityID, ComponentID> entityToComponent;
        OrderedMap<ComponentID, EntityID> componentToEntity;
        Set<ComponentID> unused;
    };

    
}
