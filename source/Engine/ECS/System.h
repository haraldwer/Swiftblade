#pragma once

#include "entity.h"
#include "utility/utility.h"

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

        virtual ComponentID Register(EntityID InObject) = 0;
        virtual ComponentID Register(EntityID InObject, const DeserializeObj& InReference) = 0;
        virtual void Unregister(EntityID InObject) = 0;
        
        virtual void InitSystem() = 0;
        virtual void UpdateSystem(double InDelta) = 0;

        virtual size_t GetComponentType() const = 0;
        SystemBase* GetAnonymousSystem(size_t InHash, bool InIsCompHash) const;

    protected:

        ComponentID Translate(EntityID InEntity) const;
        
        Map<EntityID, ComponentID> Translation;
        Vector<ComponentID> Unused; 
        ComponentID LastID = 0;
    };

    template <class T, int Size = 100>
    class System : public SystemBase
    {

    public:

        // - Implement - //  
        virtual void Init(EntityID InEntity, T& InComponent) {}
        virtual void Deinit(EntityID InEntity, T& InComponent) {}
        virtual void Update(EntityID InEntity, T& InComponent, double InDelta) {}

        virtual void InitSystem() override {}
        virtual void UpdateSystem(const double InDelta) override
        {
            for (const auto& id : Translation)
                Update(id.first, GetInternal(id.second), InDelta);
        }

        // - Helpers - //

        template <class ComponentType>
        ComponentType& Get(const EntityID InEntity)
        {
            auto ptr = TryGet<ComponentType>(InEntity);
            CHECK_ASSERT(!ptr, "Unable to find component");
            return *ptr; 
        }

        template <class ComponentType>
        ComponentType* TryGet(const EntityID InEntity)
        {
            const size_t hash = typeid(ComponentType).hash_code();
            SystemBase* base = GetAnonymousSystem(hash, true);
            System<ComponentType>* sys = reinterpret_cast<System<ComponentType>*>(base);
            return sys->TryGet(InEntity);
        }

        T* TryGet(const EntityID InEntity)
        {
            const ComponentID id = Translate(InEntity);
            CHECK_RETURN(id == InvalidID, nullptr);
            return &GetInternal(id);
        }

        template <class SystemType>
        SystemType& GetSystem() const
        {
            const size_t hash = typeid(SystemType).hash_code();
            SystemBase* base = GetAnonymousSystem(hash, true);
            CHECK_ASSERT(!base, "Unable to find system");
            return *reinterpret_cast<SystemType*>(base);
        }
        
        ComponentID Register(const EntityID InEntity) override
        {
            CHECK_ASSERT(Translate(InEntity) != InvalidID, "ID already registered");
            
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
            Translation[InEntity] = id;
            Init(InEntity, GetInternal(id));
            return id;
        }

        ComponentID Register(EntityID InObject, const DeserializeObj& InReference) override
        {
            const ComponentID compID = Register(InObject);
            T& data = GetInternal(compID);
            data.Deserialize(InReference);
            return compID;
        }
        
        void Unregister(const EntityID InEntity) override
        {
            const ComponentID id = Translate(InEntity);
            CHECK_RETURN(id == InvalidID);

            // Reset data
            T& data = GetInternal(id);
            Deinit(InEntity, GetInternal(id));
            data = T();
            Translation.erase(InEntity);
            Unused.push_back(id);
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
