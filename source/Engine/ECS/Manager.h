#pragma once
#include <vcruntime_typeinfo.h>

#include "Entity.h"
#include "System.h"
#include "Utility/Singelton.h"
#include "Utility/Utility.h"

namespace ECS
{
    class SystemBase;

    class Manager : public Utility::Singelton<Manager>
    {
    public:

        void Init();
        void Deinit(); 
        void Update(double InDelta); 

        EntityID CreateEntity();
        void DestroyEntity(EntityID InEntity);

        void DeserializeEntity(EntityID InID, const DeserializeObj& InObj);
        void SerializeEntity(EntityID InID, SerializeObj& OutObj) const; 

        template <class T>
        T& GetSystem() const
        {
            const size_t hash = typeid(T).hash_code();
            const auto find = SystemMap.find(hash);
            CHECK_ASSERT(find == SystemMap.end(), "Unable to find system");
            CHECK_ASSERT(!find->second, "System null");
            return *static_cast<T*>(find->second);
        }

        template <class T>
        T* GetComponent(const EntityID InID) const
        {
            const size_t hash = typeid(T).hash_code();
            const auto find = ComponentMap.find(hash);
            CHECK_ASSERT(find == ComponentMap.end(), "Unable to find system");
            CHECK_ASSERT(!find->second, "System null");
            System<T>* sys = reinterpret_cast<System<T>*>(find->second);
            return sys->TryGet<T>(InID);
        }

        SystemBase* GetSystem(const String& InComponentName);
        SystemBase* GetSystem(size_t InHash, bool InIsCompHash);

    private:
        
        void RegisterSystems();
        void DestroyPending(); 

        // System type -> System ptr
        Map<size_t, SystemBase*> SystemMap;
        // Component type -> System ptr
        Map<size_t, SystemBase*> ComponentMap;
        // System name -> system ptr
        Map<String, SystemBase*> NameMap;

        // List of all entities
        Set<EntityID> Entities;
        EntityID IDCounter = 0;

        // Objects that should be destroyed
        Set<EntityID> PendingDestroy; 
        
    };
}

