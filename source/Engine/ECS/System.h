#pragma once
#include <unordered_map>
#include <array>

#include "entity.h"
#include "Manager.h"
#include "utility/utility.h"

namespace ECS
{
    class Manager;

    class SystemBase
    {
        
    public:
        
        virtual ~SystemBase() = default;
        virtual void InitSystem(Manager* InOwner)
        {
            Owner = InOwner;
        }

        // Implemented by EntitySystem<T>
        virtual ComponentID Register(const EntityID InObject) { return InvalidID; }
        virtual void Unregister(const EntityID InObject) {}
        virtual void UpdateSystem(const double InDelta) {}

    protected:

        template <class T>
        T& GetSystem()
        {
            return GetManager().GetSystem<T>();
        }
        
        Manager& GetManager() const
        {
            CHECK_ASSERT(!Owner, "Invalid owner");
            return *Owner; 
        }

        ComponentID Translate(EntityID InEntity) const
        {
            const auto find = Translation.find(InEntity);
            if (find != Translation.end())
                return find->second;
            return -1;
        }

        std::unordered_map<EntityID, ComponentID> Translation;
        std::vector<ComponentID> Unused; 
        ComponentID LastID = 0;
        
    private:

        Manager* Owner = nullptr;
        
    };

    template <class T, int Size = 100>
    class System : public SystemBase
    {

    public:

        // - Implement - //  
        virtual void Init(EntityID InEntity, T& InComponent) {}
        virtual void Deinit(EntityID InEntity, T& InComponent) {}
        virtual void Update(EntityID InEntity, T& InComponent, double InDelta) {}

        virtual void UpdateSystem(const double InDelta) override
        {
            for (const auto& id : Translation)
                Update(id.first, GetInternal(id.second), InDelta);
        }

        // - Helpers - //

        // Get component from this system
        T& Get(const EntityID InEntity)
        {
            const ComponentID id = Translate(InEntity);
            CHECK_ASSERT(id == InvalidID, "No component for entity");
            return GetInternal(id);
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
        
        void Unregister(const EntityID InEntity) override
        {
            const ComponentID id = Translate(InEntity);
            CHECK_ASSERT(id == InvalidID, "ID not registered");

            // Reset data
            T& data = GetInternal(id);
            Deinit(InEntity, GetInternal(id));
            data = T();
            Translation.erase(InEntity);
            Unused.push_back(id);
        }

    private:
        
        T& GetInternal(const ComponentID InID)
        {
            CHECK_ASSERT(InID < 0 || InID > Components.size(), "Invalid index");
            return Components[InID];
        }
        
        std::array<T, Size> Components;
    };
}
