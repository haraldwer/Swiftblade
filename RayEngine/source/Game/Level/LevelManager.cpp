#include "LevelManager.h"

#include "ECS/RoomConnection.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "GameState.h"
#include "Level.h"
#include "Utility/Collections/RandomWeightedCollection.h"

void LevelManager::Load(const Vector<ResScene>& InRooms)
{
    Unload(); 
    
    Mat4F offset;
    for (const ResScene& room : InRooms)
    {
        LOG("Loading scene: " + room.Identifier().Str()); 
        
        const SceneResource* scene = room.Get();
        CHECK_CONTINUE(!scene);
        SceneInstance instance = scene->Instantiate(offset, scenes.empty());
        scenes.push_back(instance);

        for (const ECS::EntityID entity : instance.entities)
        {
            const auto connection = ECS::Manager::Get().GetComponent<RoomConnection>(entity);
            CHECK_CONTINUE(!connection);
            CHECK_CONTINUE(!connection->IsEnd);
            
            const auto endTrans = ECS::Manager::Get().GetComponent<ECS::Transform>(entity);
            CHECK_CONTINUE(!endTrans);

            // Offset is room end transform
            offset = endTrans->World();
            LOG("Found roomEnd at")
            LOG(offset);
            break;
        }
    }
}

void LevelManager::LoadLevel(const LevelConfig &InLevel)
{
    // How far has the player come?
    const auto& state = GameState::Get();
    Utility::RandomWeightedCollection<ResScene> roomCollection(InLevel.Seed);
    Utility::RandomWeightedCollection<ResScene> arenaCollection(InLevel.Seed);
    for (auto& room : InLevel.Rooms.Get())
        roomCollection.Add(room);
    for (auto& arena : InLevel.Arenas.Get())
        arenaCollection.Add(arena); 

    // Calculate scene order
    Vector<ResScene> sceneOrder;
    sceneOrder.push_back(InLevel.GameStart);
    for (int arenaIndex = 0; arenaIndex <= InLevel.NumArenas; arenaIndex++)
    {
        // Section start if not first
        if (arenaIndex > 0)
            sceneOrder.push_back(InLevel.SectionStart);

        // Rooms
        for (int roomIndex = 0; roomIndex < InLevel.NumRooms; roomIndex++)
            sceneOrder.push_back(roomCollection.Pop());

        // Maybe arena
        if (arenaIndex < InLevel.NumArenas)
        {
            sceneOrder.push_back(InLevel.SectionEnd);
            sceneOrder.push_back(arenaCollection.Pop());
        }
    }
    sceneOrder.push_back(InLevel.GameEnd);

    // Only load a subset of these
    
    // (SectionStart/GameStart + NumRooms + SectionEnd + Arena) * Checkpoint
    const int startIndex = (1 + InLevel.NumRooms + 1 + 1) * state.checkpoint;
    
    // (SectionStart/GameStart + NumRooms + SectionEnd/GameEnd + Arena) * (Checkpoint + 1)
    const int endIndex = (1 + InLevel.NumRooms + 1 + 1) * (state.checkpoint + 1) - 1; 

    if (state.arena)
    {
        // Only load end index
        Load({ sceneOrder[endIndex] });
    }
    else
    {
        Vector<ResScene> toLoad;
        for (int i = startIndex; i < endIndex; i++)
            toLoad.push_back(sceneOrder[i]);
        Load(toLoad);
    }
}

void LevelManager::Unload()
{
    for (auto& scene : scenes)
        scene.Destroy();
    scenes.clear(); 
}
