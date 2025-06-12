#include "RoomSubEditorManager.h"

#include "ECS/Volume/CubeVolume.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Input/Action.h"

void RoomSubEditorManager::Init(const RoomType InType)
{
    // Cache CubeVolume
    const auto& ecs = ECS::Manager::Get(); 
    const auto& sys = ecs.GetSystem<ECS::SysCubeVolume>();
    const auto volumes = sys.GetEntities();
    if (!volumes.empty())
    {
        cubeVolume = volumes[0];
    }
    else
    {
        if (const auto bp = ResBlueprint("Blueprints/BP_CubeVolume.json").Get())
            cubeVolume = bp->Instantiate();
    }

    // Force reset cube transform 
    if (cubeVolume != ECS::INVALID_ID)
        if (auto* cubeTrans = ecs.GetComponent<ECS::Transform>(cubeVolume))
            cubeTrans->SetWorld(Mat4F());
    
    pathEditor.SetOwner(this); 
    genEditor.SetOwner(this); 
    volumeEditor.SetOwner(this); 
    objectEditor.SetOwner(this); 

    type = InType; 
    
    pathEditor.Init(); 
    genEditor.Init(); 
    volumeEditor.Init();
    objectEditor.Init();
}

void RoomSubEditorManager::Deinit()
{
    pathEditor.Deinit();
    genEditor.Deinit();
    volumeEditor.Deinit();
    objectEditor.Deinit();
    history.Clear();
}

void RoomSubEditorManager::Update(const bool InIsCameraControlling)
{
    PROFILE();
    
    CHECK_RETURN(cubeVolume == ECS::INVALID_ID);
    
    if (InIsCameraControlling)
        GetSubEditor(mode).Update();

    // Keyboard shortcuts
    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("Undo").Pressed())
            history.Undo();
        if (Input::Action::Get("Redo").Pressed())
            history.Redo();
    }
}

void RoomSubEditorManager::Frame(const bool InIsCameraControlling)
{
    CHECK_RETURN(cubeVolume == ECS::INVALID_ID);
    GetSubEditor(mode).Frame(InIsCameraControlling);
}

void RoomSubEditorManager::DebugDraw(const bool InIsCameraControlling)
{
    CHECK_RETURN(cubeVolume == ECS::INVALID_ID);
    GetSubEditor(mode).DebugDraw(InIsCameraControlling);
}

void RoomSubEditorManager::SetMode(const SubEditorMode InMode)
{
    GetSubEditor(mode).Exit();
    mode = InMode; 
    GetSubEditor(InMode).Enter();
}

RoomSubEditor& RoomSubEditorManager::GetSubEditor(const SubEditorMode InMode)
{
    switch (InMode)
    {
    case SubEditorMode::GEN:
        return genEditor;
    case SubEditorMode::PATH:
        return pathEditor; 
    case SubEditorMode::VOLUME:
        return volumeEditor;
    case SubEditorMode::OBJECTS:
        return objectEditor;
    default:
        break;
    }
    return objectEditor; 
}

bool RoomSubEditorManager::IgnoreSave(const ECS::EntityID InID) const
{
    return InID == objectEditor.GetEditEntity(); 
}
