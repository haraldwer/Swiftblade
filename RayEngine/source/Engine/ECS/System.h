#pragma once

#include "entity.h"
#include "utility/Utility.h"

namespace ECS
{
    class Manager;
    class Component;

    class SystemBase
    {
        friend Manager; 
        
    public:
        virtual ~SystemBase() = default;

        // Implemented by EntitySystem<T>

        virtual ComponentID Register(EntityID InID) = 0;
        virtual void Unregister(EntityID InID) = 0;
        virtual size_t GetComponentType() const = 0;
        virtual void Serialize(EntityID InID, SerializeObj& OutObj) = 0; 
        virtual bool Deserialize(EntityID InID, const DeserializeObj& InObj) = 0; 
        virtual bool Edit(EntityID InID) = 0; 
        
        // Can be overriden by T
        
        virtual void InitSystem() = 0;
        virtual void UpdateSystem(double InDelta) = 0;
        virtual bool ShouldUpdate() const;

        SystemBase* GetAnonymousSystem(size_t InHash, bool InIsCompHash) const;
        bool Contains(EntityID InID) const;

    protected:

        ComponentID Translate(EntityID InID) const;
        
        Map<EntityID, ComponentID> Translation;
        Vector<ComponentID> Unused; 
        ComponentID LastID = 0;
    };

    template <class T, int Size = 100>
    class System : public SystemBase
    {

    public:

        // - Implement - //  
        virtual void Init(EntityID InID, T& InComponent) {}
        virtual void Deinit(EntityID InID, T& InComponent) {}
        virtual void Update(EntityID InID, T& InComponent, double InDelta) {}

        void InitSystem() override {}
        void UpdateSystem(const double InDelta) override
        {
            for (const auto& id : Translation)
                Update(id.first, GetInternal(id.second), InDelta);
        }

        // - Helpers - //

        template <class ComponentType>
        ComponentType& Get(const EntityID InID)
        {
            auto ptr = TryGet<ComponentType>(InID);
            CHECK_ASSERT(!ptr, "Unable to find component");
            return *ptr; 
        }

        template <class ComponentType>
        ComponentType* TryGet(const EntityID InID)
        {
            const size_t hash = typeid(ComponentType).hash_code();
            SystemBase* base = GetAnonymousSystem(hash, true);
            System<ComponentType>* sys = reinterpret_cast<System<ComponentType>*>(base);
            return sys->TryGet(InID);
        }

        T* TryGet(const EntityID InID)
        {
            const ComponentID id = Translate(InID);
            CHECK_RETURN(id == InvalidID, nullptr);
            return &GetInternal(id);
        }

        template <class SystemType>
        SystemType& GetSystem() const
        {
            const size_t hash = typeid(SystemType).hash_code();
            SystemBase* base = GetAnonymousSystem(hash, false);
            CHECK_ASSERT(!base, "Unable to find system");
            return *reinterpret_cast<SystemType*>(base);
        }
        
        ComponentID Register(const EntityID InID) override
        {
            CHECK_ASSERT(Translate(InID) != InvalidID, "ID already registered");
            
            // Find ID
            const auto findID = [&]()
            {
                if (Unused.size())
                {
                    const ComponentID id = Unused.back();
                    Unused.pop_back();
                    return id; 
                }

                const ComponentID newID = LastID + 1;
                CHECK_ASSERT(newID >= Size, "Out of IDs, increase size!");
                LastID = newID;
                return newID;
            };
            
            // Register
            const ComponentID id = findID();
            Translation[InID] = id;
            Init(InID, GetInternal(id));
            return id;
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
            T& data = GetInternal(InID);
            data.Serialize(OutObj);
        }

        bool Deserialize(EntityID InID, const DeserializeObj& InObj) override
        {
            T& data = GetInternal(InID);
            return data.Deserialize(InObj);
        }

        bool Edit(EntityID InID) override
        {
            T& data = GetInternal(InID);
            return data.Edit(); 
        }

        size_t GetComponentType() const override
        {
            return typeid(T).hash_code();
        }

    private:
        
        T& GetInternal(const ComponentID InID)
        {
            CHECK_ASSERT(InID < 0 || InID > Components.size(), "Invalid index");
            return Components[InID];
        }

        // Components are never moved in memory
        Array<T, Size> Components;
    };
}
