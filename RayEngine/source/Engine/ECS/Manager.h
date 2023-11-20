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

        template <class T>
        T& GetSystem() const
        {
            const Utility::TypeHash hash = Utility::Type<T>().GetHash();
            const auto find = SystemMap.find(hash);
            CHECK_ASSERT(find == SystemMap.end(), "Unable to find system");
            CHECK_ASSERT(!find->second, "System null");
            return *static_cast<T*>(find->second);
        }

        template <class T>
        T* GetComponent(const EntityID InID) const
        {
            const Utility::TypeHash hash = Utility::Type<T>().GetHash();
            const auto find = ComponentMap.find(hash);
            CHECK_ASSERT(find == ComponentMap.end(), "Unable to find system");
            CHECK_ASSERT(!find->second, "System null");
            auto* sys = reinterpret_cast<System<T>*>(find->second);
            return sys->TryGet(InID);
        }

        SystemBase* GetSystem(const String& InComponentName);
        SystemBase* GetSystem(size_t InHash, bool InIsCompHash);
        
        const Map<String, SystemBase*>& GetAllSystems() const { return NameMap; }

        void Deserialize(EntityID InID, const Vector<DeserializeObj>& InObjects);
        void Serialize(EntityID InID, SerializeObj& OutObj);

    private:

        struct DeserializeSysTuple
        {
            Set<SystemBase*> Systems;
            int Depth = 0;
        };
        typedef Map<EntityID, DeserializeSysTuple> DeserializeSysCollection;
        
        void Deserialize(EntityID InID, const DeserializeObj& InObj, DeserializeSysCollection& OutSystems, int InDepth); 
        Set<SystemBase*> DeserializeComponents(EntityID InID, const DeserializeObj& InObj); 
        void DeserializeChildren(EntityID InID, const DeserializeObj& InObj, DeserializeSysCollection& OutSystems, int InDepth);
        
        void RegisterSystems();
        void DestroyPending(); 

        // System type -> System ptr
        Map<Utility::TypeHash, SystemBase*> SystemMap;
        // Component type -> System ptr
        Map<Utility::TypeHash, SystemBase*> ComponentMap;
        // System name -> system ptr
        Map<String, SystemBase*> NameMap;

        // List of all entities
        Set<EntityID> Entities;
        EntityID IDCounter = 0;

        // Objects that should be destroyed
        Set<EntityID> PendingDestroy; 
        
    };
}

