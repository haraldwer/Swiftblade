#pragma once
#include <set>
#include <unordered_map>
#include <vcruntime_typeinfo.h>

#include "entity.h"
#include "utility/singelton.h"
#include "utility/utility.h"

namespace ECS
{
    class SystemBase;

    class Manager : public Singelton<Manager>
    {
    public:

        void Init();
        void Deinit(); 
        void Update(double InDelta); 

        EntityID CreateEntity();
        void DestroyEntity(EntityID InEntity);

        template <class T>
        T& GetSystem()
        {
            const size_t hash = typeid(T).hash_code();
            const auto find = Systems.find(hash);
            CHECK_ASSERT(find == Systems.end(), "Unable to find component");
            CHECK_ASSERT(!find->second, "Component null");
            return *static_cast<T*>(find->second);
        }

    private:
        
        void RegisterSystems();
        void DestroyPending();

        // Map of type to component
        std::unordered_map<size_t, SystemBase*> Systems;

        // List of all entities
        std::set<EntityID> Entities;
        EntityID IDCounter = 0;

        // Objects that should be destroyed
        std::set<EntityID> PendingDestroy; 
        
    };
}

