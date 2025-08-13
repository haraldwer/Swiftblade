#include "RoomSubEditorManager.h"

#include "Engine/ECS/Manager.h"
#include "SubEditors/RoomGenEditor.h"
#include "SubEditors/RoomObjectEditor.h"
#include "SubEditors/RoomPathEditor.h"
#include "SubEditors/RoomVolumeEditor.h"

#define CREATE_SUBEDITOR(x) \
editors[Type::Get<x>()] = x();

void RoomSubEditorManager::Init()
{
    CREATE_SUBEDITOR(RoomConnectionEditor)
    CREATE_SUBEDITOR(RoomPathEditor)
    CREATE_SUBEDITOR(RoomGenEditor)
    CREATE_SUBEDITOR(RoomObjectEditor)
    CREATE_SUBEDITOR(RoomVolumeEditor)
    
    for (auto& e : editors)
        e.second.Get().SetOwner(this);
    for (auto& e : editors)
        e.second.Get().Init();
}

#undef CREATE_SUBEDITOR

void RoomSubEditorManager::Deinit()
{
    for (auto& e : editors)
        e.second.Get().Deinit();
}

void RoomSubEditorManager::Update()
{
    for (auto& e : editors)
        e.second.Get().Update();
}

void RoomSubEditorManager::Frame()
{
    for (auto& e : editors)
        e.second.Get().Frame();
}

void RoomSubEditorManager::DebugDraw()
{
    for (auto& e : editors)
        e.second.Get().Frame();
}

void RoomSubEditorManager::SetCurrent(const Type &InType)
{
    CHECK_RETURN(currentEditor == InType);
    if (editors.contains(currentEditor))
        editors.at(currentEditor).Get().Exit();
    currentEditor = InType;
    editors.at(currentEditor).Get().Enter();
}

bool RoomSubEditorManager::IgnoreSave(const ECS::EntityID InID)
{
    for (auto& e : editors)
        if (e.second.Get().IgnoreSave(InID))
            return true;
    return false;
}
