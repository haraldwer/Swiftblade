#include "RoomManager.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Game/ECS/RoomConnection.h"
#include "Game/GameState.h"
#include "RoomConfig.h"
#include "Utility/Math/Random.hpp"
#include "Utility/RandomWeightedCollection.h"

void RoomManager::Load(const Vector<ResScene>& InRooms, bool InApplyRootOffset)
{
    Unload(); 
    
    Mat4F offset;
    for (const ResScene& room : InRooms)
    {
        LOG("Loading scene: " + room.Identifier()); 
        
        const Scene* scene = room.Get();
        CHECK_CONTINUE(!scene);
        SceneInstance instance = scene->Create(offset, Scenes.empty() && !InApplyRootOffset);
        Scenes.push_back(instance);

        bool foundEnd = false; 
        for (const ECS::EntityID entity : instance.Entities)
        {
            const auto roomEnd = ECS::Manager::Get().GetComponent<RoomConnection>(entity);
            CHECK_CONTINUE(!roomEnd);
            CHECK_CONTINUE(!roomEnd->IsEnd);
            const auto roomEndTrans = ECS::Manager::Get().GetComponent<ECS::Transform>(entity);
            CHECK_CONTINUE(!roomEndTrans);

            // Offset is room end transform
            offset = roomEndTrans->World();
            foundEnd = true; 
            break; 
        }

        if (!foundEnd)
            offset.SetPosition(offset.GetPosition() + Vec3F(0.0f, 0.0f, 50.0f));
    }
}

void RoomManager::LoadConfig()
{
    RoomConfig config;
    config.Load("Scenes/Configs/C_Default.json");

    // How far has the player come?
    const auto& state = GameState::Get();
    Utility::RandomWeightedCollection<ResScene> roomCollection(state.Seed);
    Utility::RandomWeightedCollection<ResScene> arenaCollection(state.Seed);
    for (auto& room : config.Rooms.Get())
        roomCollection.Add(room);
    for (auto& arena : config.Arenas.Get())
        arenaCollection.Add(arena); 

    // Calculate scene order
    Vector<ResScene> scenes;
    scenes.push_back(config.GameStart);
    for (int arenaIndex = 0; arenaIndex <= config.NumArenas; arenaIndex++)
    {
        // Section start if not first
        if (arenaIndex > 0)
            scenes.push_back(config.SectionStart);

        // Rooms
        for (int roomIndex = 0; roomIndex < config.NumRooms; roomIndex++)
            scenes.push_back(roomCollection.Pop());

        // Maybe arena
        if (arenaIndex < config.NumArenas)
        {
            scenes.push_back(config.SectionEnd);
            scenes.push_back(arenaCollection.Pop());
        }
    }
    scenes.push_back(config.GameEnd);

    // Only load a subset of these
    
    // (SectionStart/GameStart + NumRooms + SectionEnd + Arena) * Checkpoint
    const int startIndex = (1 + config.NumRooms + 1 + 1) * state.Checkpoint;
    
    // (SectionStart/GameStart + NumRooms + SectionEnd/GameEnd + Arena) * (Checkpoint + 1)
    const int endIndex = (1 + config.NumRooms + 1 + 1) * (state.Checkpoint + 1) - 1; 

    if (state.InArena)
    {
        // Only load end index
        Load({ scenes[endIndex] }, true);
    }
    else
    {
        Vector<ResScene> toLoad;
        for (int i = startIndex; i < endIndex; i++)
            toLoad.push_back(scenes[i]);
        Load(toLoad, true);
    }
}

void RoomManager::Unload()
{
    for (auto& scene : Scenes)
        scene.Destroy();
    Scenes.clear(); 
}
