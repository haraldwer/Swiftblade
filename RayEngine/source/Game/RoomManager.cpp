#include "RoomManager.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Systems/RoomEnd.h"

void RoomManager::Load(const Vector<ResScene>& InRooms)
{
    Unload(); 
    
    Mat4F offset; 
    for (const ResScene& room : InRooms)
    {
        const Scene* scene = room.Get();
        CHECK_CONTINUE(!scene);
        SceneInstance instance = scene->Create(offset); 
        Scenes.push_back(instance);

        bool foundEnd = false; 
        for (const ECS::EntityID entity : instance.Entities)
        {
            const auto roomEnd = ECS::Manager::Get().GetComponent<RoomEnd>(entity);
            CHECK_CONTINUE(!roomEnd);
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

void RoomManager::Unload()
{
    for (auto& scene : Scenes)
        scene.Destroy();
    Scenes.clear(); 
}
