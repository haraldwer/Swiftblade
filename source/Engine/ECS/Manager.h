#pragma once
#include <vcruntime_typeinfo.h>

#include "Entity.h"
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
        T& GetSystem()
        {
            const size_t hash = typeid(T).hash_code();
            const auto find = Systems.find(hash);
            CHECK_ASSERT(find == Systems.end(), "Unable to find component");
            CHECK_ASSERT(!find->second.Get(), "Component null");
            return *static_cast<T*>(find->second.Get());
        }

    private:
        
        void RegisterSystems();
        void DestroyPending();

        // Map of type to component
        Map<size_t, ObjectPtr<SystemBase>> Systems;

        // List of all entities
        Set<EntityID> Entities;
        EntityID IDCounter = 0;

        // Objects that should be destroyed
        Set<EntityID> PendingDestroy; 
        
    };
}

