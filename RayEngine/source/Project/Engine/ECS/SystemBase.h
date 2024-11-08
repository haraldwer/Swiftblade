#pragma once

#include "entity.h"
#include "utility/Utility.h"

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
        virtual void SystemUpdate() = 0;
        virtual void SystemFrame() = 0;
        virtual bool ShouldUpdate() const;
        virtual void OnBeginContact(const Physics::Contact& InContact) {}
        virtual void OnEndContact(const Physics::Contact& InContact) {}
        virtual int GetPriority() const { return 0; }

        static SystemBase* GetAnonymousSystem(const Utility::Type& InType, bool InIsCompHash);
        bool Contains(EntityID InID) const;
        Vector<EntityID> GetEntities() const;

    protected:

        ComponentID Translate(EntityID InID) const;
        
        Map<EntityID, ComponentID> Translation;
        Vector<ComponentID> Unused;
    };
}