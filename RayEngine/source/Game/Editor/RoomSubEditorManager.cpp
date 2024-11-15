#include "RoomSubEditorManager.h"

#include "ECS/Volume/CubeVolume.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Input/Action.h"

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
    
    PathEditor.SetOwner(this); 
    GenEditor.SetOwner(this); 
    VolumeEditor.SetOwner(this); 
    ObjectEditor.SetOwner(this); 

    Type = InType; 
    
    PathEditor.Init(); 
    GenEditor.Init(); 
    VolumeEditor.Init();
    ObjectEditor.Init();
}

void RoomSubEditorManager::Deinit()
{
    PathEditor.Deinit();
    GenEditor.Deinit();
    VolumeEditor.Deinit();
    ObjectEditor.Deinit();
    History.Clear();
}

void RoomSubEditorManager::Update(const bool InIsCameraControlling)
{
    CHECK_RETURN(CubeVolume == ECS::InvalidID);
    
    if (InIsCameraControlling)
        GetSubEditor(Mode).Update();

    // Keyboard shortcuts
    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("Undo").Pressed())
            History.Undo();
        if (Input::Action::Get("Redo").Pressed())
            History.Redo();
    }
}

void RoomSubEditorManager::Frame(const bool InIsCameraControlling)
{
    CHECK_RETURN(CubeVolume == ECS::InvalidID);
    GetSubEditor(Mode).Frame(InIsCameraControlling);
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
    case SubEditorMode::GEN:
        return GenEditor;
    case SubEditorMode::PATH:
        return PathEditor; 
    case SubEditorMode::VOLUME:
        return VolumeEditor;
    case SubEditorMode::OBJECTS:
        return ObjectEditor; 
    }
    return ObjectEditor; 
}

bool RoomSubEditorManager::IgnoreSave(const ECS::EntityID InID) const
{
    return InID == ObjectEditor.GetEditEntity(); 
}
