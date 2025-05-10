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
            for (const auto& id : Translation)
                Update(id.first, GetInternal(id.second));
        }
        void SystemFrame() override
        {
            for (const auto& id : Translation)
                Frame(id.first, GetInternal(id.second));
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
            CHECK_RETURN(id == InvalidID, nullptr);
            return &GetInternal(id);
        }

        const T* TryGet(const EntityID InID) const
        {
            const ComponentID id = Translate(InID);
            CHECK_RETURN(id == InvalidID, nullptr);
            return &GetInternal(id);
        }

        template <class SystemType>
        SystemType& GetSystem() const
        {
            SystemBase* base = GetAnonymousSystem(Type::Get<SystemType>(), false);
            CHECK_ASSERT(!base, "Unable to find system");
            return *reinterpret_cast<SystemType*>(base);
        }
        
        ComponentID Register(const EntityID InID, bool const InDeferredInit) override
        {
            // Already registered? 
            const ComponentID existingID = Translate(InID);
            CHECK_RETURN(existingID != InvalidID, existingID);
            
            // Find ID
            const auto findID = [&]()
            {
                if (Unused.size())
                {
                    const ComponentID id = Unused.back();
                    Unused.pop_back();
                    return id; 
                }
                
                const auto newID = static_cast<ComponentID>(Components.size());
                Components.emplace_back();
                return newID;
            };
            
            // Register
            const ComponentID id = findID();
            Translation[InID] = id;

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
            CHECK_ASSERT(id == InvalidID, "Invalid ID");
            T& data = GetInternal(id);
            Init(InID, data);
        }
        
        void Unregister(const EntityID InID) override
        {
            const ComponentID id = Translate(InID);
            CHECK_RETURN(id == InvalidID);

            // Reset data
            T& data = GetInternal(id);
            Deinit(InID, GetInternal(id));
            data = T();
            Translation.erase(InID);
            Unused.push_back(id);
        }

        void Serialize(EntityID InID, SerializeObj& OutObj) override
        {
            const ComponentID id = Translate(InID);
            CHECK_ASSERT(id == InvalidID, "Invalid ID");
            T& data = GetInternal(id);
            data.Serialize(OutObj);
        }

        bool Deserialize(EntityID InID, const DeserializeObj& InObj) override
        {
            const ComponentID id = Translate(InID);
            CHECK_ASSERT(id == InvalidID, "Invalid ID");
            T& data = GetInternal(id);
            return data.Deserialize(InObj);
        }

        bool Edit(EntityID InID) override
        {
            const ComponentID id = Translate(InID);
            CHECK_ASSERT(id == InvalidID, "Invalid ID");
            T& data = GetInternal(id);
            return data.Edit(); 
        }

    protected:
        
        T& GetInternal(const ComponentID InID)
        {
            CHECK_ASSERT(InID < 0 || InID > Components.size(), "Invalid index");
            return Components[InID];
        }

        const T& GetInternal(const ComponentID InID) const
        {
            CHECK_ASSERT(InID < 0 || InID > Components.size(), "Invalid index");
            return Components[InID];
        }

    private:
        
        Vector<T> Components;
    };
}
