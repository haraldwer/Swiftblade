#pragma once

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
        void Update(); 
        void Frame(); 

        EntityID CreateEntity();
        void DestroyEntity(EntityID InEntity);
        void DestroyPending();

        template <class T>
        T& GetSystem() const
        {
            const Utility::TypeHash hash = Utility::GetType<T>();
            const auto find = SystemMap.find(hash);
            CHECK_ASSERT(find == SystemMap.end(), "Unable to find system");
            CHECK_ASSERT(!find->second, "System null");
            return *static_cast<T*>(find->second);
        }

        template <class T>
        T* GetComponent(const EntityID InID) const
        {
            const Utility::TypeHash hash = Utility::GetType<T>();
            const auto find = ComponentMap.find(hash);
            CHECK_ASSERT(find == ComponentMap.end(), "Unable to find system");
            CHECK_ASSERT(!find->second, "System null");
            auto* sys = reinterpret_cast<System<T>*>(find->second);
            return sys->TryGet(InID);
        }

        SystemBase* GetSystem(const String& InComponentName);
        SystemBase* GetSystem(const Utility::Type& InType, bool InIsCompHash);
        
        const Map<String, SystemBase*>& GetAllSystems() const { return NameToSystem; }
        std::set<EntityID> GetAllEntities() { return Entities; }

        void Deserialize(EntityID InID, const Mat4F& InTransform, const Vector<DeserializeObj>& InObjects);
        void Serialize(EntityID InID, SerializeObj& OutObj, bool InChildren);

    private:

        struct DeserializeSysTuple
        {
            Set<SystemBase*> Systems;
            int Depth = 0;
        };
        typedef Map<EntityID, DeserializeSysTuple> DeserializeEntityCollection;
        
        void Deserialize(EntityID InID, const DeserializeObj& InObj, DeserializeEntityCollection& OutSystems, int InDepth); 
        Set<SystemBase*> DeserializeComponents(EntityID InID, const DeserializeObj& InObj); 
        void DeserializeChildren(EntityID InID, const DeserializeObj& InObj, DeserializeEntityCollection& OutSystems, int InDepth);
        
        void RegisterSystems();
        void SortSystems(); 

        // System type -> System ptr
        Map<Utility::TypeHash, SystemBase*> SystemMap;
        // Component type -> System ptr
        Map<Utility::TypeHash, SystemBase*> ComponentMap;
        // System name -> system ptr
        Map<String, SystemBase*> NameToSystem;
        Map<SystemBase*, String> SystemToName;
        Vector<SystemBase*> SortedSystems; 

        // List of all entities
        Set<EntityID> Entities;
        EntityID IDCounter = 0;

        // Objects that should be destroyed
        Set<EntityID> PendingDestroy; 
        
    };
}

