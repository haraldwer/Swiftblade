#include "LevelManager.h"

#include "ECS/RoomConnection.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "GameState.h"
#include "Level.h"
#include "Utility/Collections/RandomWeightedCollection.h"

void LevelManager::Load(const Vector<ResScene>& InRooms, bool InApplyRootOffset)
{
    Unload(); 
    
    Mat4F offset;
    for (const ResScene& room : InRooms)
    {
        LOG("Loading scene: " + room.Identifier().Str()); 
        
        const SceneResource* scene = room.Get();
        CHECK_CONTINUE(!scene);
        SceneInstance instance = scene->Instantiate(offset, scenes.empty() && !InApplyRootOffset);
        scenes.push_back(instance);

        bool foundEnd = false; 
        for (const ECS::EntityID entity : instance.entities)
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

void LevelManager::LoadConfig()
{
    Level config;
    config.Load("Scenes/Configs/C_Default.json");

    // How far has the player come?
    const auto& state = GameState::Get();
    Utility::RandomWeightedCollection<ResScene> roomCollection(state.seed);
    Utility::RandomWeightedCollection<ResScene> arenaCollection(state.seed);
    for (auto& room : config.Rooms.Get())
        roomCollection.Add(room);
    for (auto& arena : config.Arenas.Get())
        arenaCollection.Add(arena); 

    // Calculate scene order
    Vector<ResScene> sceneOrder;
    sceneOrder.push_back(config.GameStart);
    for (int arenaIndex = 0; arenaIndex <= config.NumArenas; arenaIndex++)
    {
        // Section start if not first
        if (arenaIndex > 0)
            sceneOrder.push_back(config.SectionStart);

        // Rooms
        for (int roomIndex = 0; roomIndex < config.NumRooms; roomIndex++)
            sceneOrder.push_back(roomCollection.Pop());

        // Maybe arena
        if (arenaIndex < config.NumArenas)
        {
            sceneOrder.push_back(config.SectionEnd);
            sceneOrder.push_back(arenaCollection.Pop());
        }
    }
    sceneOrder.push_back(config.GameEnd);

    // Only load a subset of these
    
    // (SectionStart/GameStart + NumRooms + SectionEnd + Arena) * Checkpoint
    const int startIndex = (1 + config.NumRooms + 1 + 1) * state.checkpoint;
    
    // (SectionStart/GameStart + NumRooms + SectionEnd/GameEnd + Arena) * (Checkpoint + 1)
    const int endIndex = (1 + config.NumRooms + 1 + 1) * (state.checkpoint + 1) - 1; 

    if (state.arena)
    {
        // Only load end index
        Load({ sceneOrder[endIndex] }, true);
    }
    else
    {
        Vector<ResScene> toLoad;
        for (int i = startIndex; i < endIndex; i++)
            toLoad.push_back(sceneOrder[i]);
        Load(toLoad, true);
    }
}

void LevelManager::Unload()
{
    for (auto& scene : scenes)
        scene.Destroy();
    scenes.clear(); 
}
