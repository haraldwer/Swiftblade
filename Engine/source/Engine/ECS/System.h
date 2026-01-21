#pragma once

#include "Entity.h"
#include "SystemBase.h"

namespace ECS
{
    template <class T>
    class System : public SystemBase
    {

    public:

        // - Implement - //  
        virtual void Init(EntityID InID, T& InComponent) {}
        virtual void Deinit(EntityID InID, T& InComponent) {}
        virtual void Update(EntityID InID, T& InComponent) {}
        virtual void Frame(EntityID InID, T& InComponent) {}

        void SystemInit() override {}
        void SystemUpdate() override
        {
            for (const auto& id : ComponentMap())
                Update(id.second, GetInternal(id.first));
        }
        void SystemFrame() override
        {
            for (const auto& id : ComponentMap())
                Frame(id.second, GetInternal(id.first));
        }

        // - Helpers - //

        template <class ComponentType = T>
        ComponentType& Get(const EntityID InID)
        {
            auto ptr = TryGet<ComponentType>(InID);
            CHECK_ASSERT(!ptr, "Unable to find component");
            return *ptr; 
        }

        template <class ComponentType = T>
        const ComponentType& Get(const EntityID InID) const
        {
            auto ptr = TryGet<ComponentType>(InID);
            CHECK_ASSERT(!ptr, "Unable to find component");
            return *ptr; 
        }

        template <class ComponentType = T>
        ComponentType* TryGet(const EntityID InID)
        {
            SystemBase* base = GetAnonymousSystem(Type::Get<ComponentType>(), true);
            System<ComponentType>* sys = reinterpret_cast<System<ComponentType>*>(base);
            return sys->TryGet(InID);
        }

        template <class ComponentType = T>
        const ComponentType* TryGet(const EntityID InID) const
        {
            SystemBase* base = GetAnonymousSystem(Type::Get<ComponentType>(), true);
            System<ComponentType>* sys = reinterpret_cast<System<ComponentType>*>(base);
            return sys->TryGet(InID);
        }

        T* TryGet(const EntityID InID)
        {
            const ComponentID id = Translate(InID);
            CHECK_RETURN(id == INVALID_ID, nullptr);
            return &GetInternal(id);
        }

        const T* TryGet(const EntityID InID) const
        {
            const ComponentID id = Translate(InID);
            CHECK_RETURN(id == INVALID_ID, nullptr);
            return &GetInternal(id);
        }

        template <class SystemType>
        static SystemType& GetSystem()
        {
            SystemBase* base = GetAnonymousSystem(Type::Get<SystemType>(), false);
            CHECK_ASSERT(!base, "Unable to find system");
            return *reinterpret_cast<SystemType*>(base);
        }
        
        ComponentID Register(const EntityID InID, bool const InDeferredInit = false) override
        {
            // Already registered? 
            const ComponentID existingID = Translate(InID);
            CHECK_RETURN(existingID != INVALID_ID, existingID);
            
            // Register
            const ComponentID id = RegisterID(InID);

            // Init comp
            T& data = GetInternal(id);
            data.SetID(InID);
            if (!InDeferredInit)
                FinishRegistration(InID);
            
            return id;
        }

        void FinishRegistration(const EntityID InID) override
        {
            const ComponentID id = Translate(InID);
            CHECK_ASSERT(id == INVALID_ID, "Invalid ID");
            T& data = GetInternal(id);
            Init(InID, data);
        }
        
        void Unregister(const EntityID InID) override
        {
            const ComponentID id = Translate(InID);
            CHECK_RETURN(id == INVALID_ID);

            // Reset data
            T& data = GetInternal(id);
            Deinit(InID, GetInternal(id));
            data = T();

            UnregisterID(InID, id);
        }

        void Serialize(EntityID InID, SerializeObj& OutObj) override
        {
            const ComponentID id = Translate(InID);
            CHECK_ASSERT(id == INVALID_ID, "Invalid ID");
            T& data = GetInternal(id);
            data.Serialize(OutObj);
        }

        bool Deserialize(EntityID InID, const DeserializeObj& InObj) override
        {
            const ComponentID id = Translate(InID);
            CHECK_ASSERT(id == INVALID_ID, "Invalid ID");
            T& data = GetInternal(id);
            return data.Deserialize(InObj);
        }

        bool Edit(EntityID InID) override
        {
            const ComponentID id = Translate(InID);
            CHECK_ASSERT(id == INVALID_ID, "Invalid ID");
            T& data = GetInternal(id);
            return data.Edit(); 
        }

    protected:
        
        T& GetInternal(const ComponentID InID)
        {
            CHECK_ASSERT(InID < 0 || InID > components.size(), "Invalid index");
            return components[InID];
        }

        const T& GetInternal(const ComponentID InID) const
        {
            CHECK_ASSERT(InID < 0 || InID > components.size(), "Invalid index");
            return components[InID];
        }

        ComponentID NewID() override
        {
            components.emplace_back();
            return static_cast<ComponentID>(components.size() - 1);
        }

    private:
        
        Vector<T> components;
    };
}
