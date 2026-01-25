#pragma once

#include "Entity.h"
#include "System.h"

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
            const Utility::TypeHash hash = Type::Get<T>().GetHash();
            const auto find = systemMap.find(hash);
            CHECK_ASSERT(find == systemMap.end(), "Unable to find system");
            CHECK_ASSERT(!find->second, "System null");
            return *static_cast<T*>(find->second);
        }

        template <class T>
        T* GetComponent(const EntityID InID) const
        {
            const Utility::TypeHash hash = Type::Get<T>().GetHash();
            const auto find = componentMap.find(hash);
            CHECK_ASSERT(find == componentMap.end(), "Unable to find system");
            CHECK_ASSERT(!find->second, "System null");
            auto* sys = reinterpret_cast<System<T>*>(find->second);
            return sys->TryGet(InID);
        }

        SystemBase* GetSystem(const String& InComponentName);
        SystemBase* GetSystem(const Utility::Type& InType, bool InIsCompHash);
        void RegisterSystem(SystemBase* InSys, const String& InName, Utility::TypeHash InComponentHash, Utility::TypeHash InSystemHash);
        
        const Map<String, SystemBase*>& GetAllSystems() const { return nameToSystem; }
        std::set<EntityID> GetAllEntities() { return entities; }

        void Deserialize(EntityID InID, const Mat4F& InTransform, const Vector<DeserializeObj>& InObjects);
        void Serialize(EntityID InID, SerializeObj& OutObj, bool InChildren);
    
    private:

        struct DeserializeSysTuple
        {
            Set<SystemBase*> systems;
            int depth = 0;
        };
        typedef Map<EntityID, DeserializeSysTuple> DeserializeEntityCollection;
        
        void Deserialize(EntityID InID, const DeserializeObj& InObj, DeserializeEntityCollection& OutSystems, int InDepth); 
        Set<SystemBase*> DeserializeComponents(EntityID InID, const DeserializeObj& InObj); 
        void DeserializeChildren(EntityID InID, const DeserializeObj& InObj, DeserializeEntityCollection& OutSystems, int InDepth);
        
        void SortSystems(); 

        // System type -> System ptr
        Map<Utility::TypeHash, SystemBase*> systemMap = {};
        // Component type -> System ptr
        Map<Utility::TypeHash, SystemBase*> componentMap = {};
        // System name -> system ptr
        Map<String, SystemBase*> nameToSystem = {};
        Map<SystemBase*, String> systemToName = {};
        Vector<SystemBase*> sortedSystems = {}; 

        // List of all entities
        Set<EntityID> entities = {};
        EntityID idCounter = 0;

        // Objects that should be destroyed
        Set<EntityID> pendingDestroy = {};
    };
}

