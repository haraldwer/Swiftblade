#include "RoomSubEditorManager.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Game/ECS/Volume/CubeVolume.h"

void RoomSubEditorManager::Init(const RoomType InType)
{
    // Cache CubeVolume
    auto& ecs = ECS::Manager::Get(); 
    const auto& sys = ecs.GetSystem<ECS::SysCubeVolume>();
    const auto volumes = sys.GetEntities();
    if (!volumes.empty())
    {
        CubeVolume = volumes[0];
    }
    else
    {
        if (const auto bp = ResBlueprint("Blueprints/BP_CubeVolume.json").Get())
            CubeVolume = bp->Instantiate();
    }

    // Force reset cube transform 
    if (CubeVolume != ECS::InvalidID)
        if (auto* cubeTrans = ecs.GetComponent<ECS::Transform>(CubeVolume))
            cubeTrans->SetWorld(Mat4F());
    
    VolumeEditor.SetOwner(this); 
    ObjectEditor.SetOwner(this); 
    ConnectionEditor.SetOwner(this); 

    Type = InType; 
    
    VolumeEditor.Init();
    ObjectEditor.Init();
    ConnectionEditor.Init(); 
}

void RoomSubEditorManager::Deinit()
{
    VolumeEditor.Deinit();
    ObjectEditor.Deinit();
    ConnectionEditor.Deinit();
    History.Clear();
}

void RoomSubEditorManager::Update(const double InDelta, const bool InIsCameraControlling)
{
    CHECK_RETURN(CubeVolume == ECS::InvalidID);
    
    if (InIsCameraControlling)
        GetSubEditor(Mode).Update(InDelta);

    // Keyboard shortcuts
    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_Z))
            History.Undo();
        if (IsKeyPressed(KEY_Y))
            History.Redo();
    }
}

void RoomSubEditorManager::UpdateUI(const bool InIsCameraControlling)
{
    CHECK_RETURN(CubeVolume == ECS::InvalidID);
    GetSubEditor(Mode).UpdateUI(InIsCameraControlling);
}

void RoomSubEditorManager::DebugDraw(bool InIsCameraControlling)
{
    CHECK_RETURN(CubeVolume == ECS::InvalidID);
    GetSubEditor(Mode).DebugDraw(InIsCameraControlling);
}

void RoomSubEditorManager::SetMode(const SubEditorMode InMode)
{
    GetSubEditor(Mode).Exit();
    Mode = InMode; 
    GetSubEditor(InMode).Enter();
}

RoomSubEditor& RoomSubEditorManager::GetSubEditor(const SubEditorMode InMode)
{
    switch (InMode)
    {
    case SubEditorMode::VOLUME:
        return VolumeEditor;
    case SubEditorMode::OBJECTS:
        return ObjectEditor; 
    case SubEditorMode::CONNECTIONS:
        return ConnectionEditor; 
    }
    return VolumeEditor; 
}

bool RoomSubEditorManager::IgnoreSave(const ECS::EntityID InID) const
{
    return InID == ObjectEditor.GetEditEntity(); 
}
