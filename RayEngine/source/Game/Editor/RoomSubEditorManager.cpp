#include "RoomSubEditorManager.h"

#include "RoomEditor.h"
#include "Engine/ECS/Manager.h"
#include "Input/Action.h"
#include "Menus/MenuRoomEditor.h"
#include "SubEditors/RoomGenEditor.h"
#include "SubEditors/RoomObjectEditor.h"
#include "SubEditors/RoomPathEditor.h"
#include "SubEditors/RoomVolumeEditor.h"
#include "SubEditors/RoomConnectionEditor.h"

#define CREATE_SUBEDITOR(x, name) \
editors[Type::GetHash<x>()] = x(); \
menu.AddOption(#x, name)

void RoomSubEditorManager::Init(RoomEditor* InEditor)
{
    CHECK_ASSERT(!InEditor, "Invalid editor")

    auto& menu = InEditor->GetMenu();
    
    CREATE_SUBEDITOR(RoomConnectionEditor, "Connection");
    CREATE_SUBEDITOR(RoomPathEditor, "Path");
    CREATE_SUBEDITOR(RoomGenEditor, "Generate");
    CREATE_SUBEDITOR(RoomVolumeEditor, "Block");
    CREATE_SUBEDITOR(RoomObjectEditor, "Objects");
    
    for (auto& e : editors)
        e.second.Get().editor = InEditor;
    for (auto& e : editors)
        e.second.Get().Init();

    auto& room = InEditor->GetRoom();
    if (room.Connection == ECS::VolumeCoord().key)
        SetCurrent(Type::Get<RoomConnectionEditor>());
    else if (room.Path.Get().empty())
        SetCurrent(Type::Get<RoomPathEditor>());
    else if (room.volumeData.data.empty())
        SetCurrent(Type::Get<RoomGenEditor>());
    else SetCurrent(Type::Get<RoomObjectEditor>());
    
    onMenuClicked.Bind([&](auto& e, auto c)
    {
        SetCurrent(e.InOption);
    });
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
        if (Input::Action::Get(e.second.Get().GetObjName(), "RoomEditor").Pressed())
            SetCurrent(Type(e.first));
    
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
    String editorName = editors.at(GetCurrent().GetHash()).Get<>().GetObjName();
    ImGui::Text("Current room: %s", editorName.c_str());
    for (auto& e : editors)
        e.second.Get().Frame();
}

void RoomSubEditorManager::SetCurrent(const Type &InType)
{
    CHECK_RETURN(currentEditor == InType);
    if (editors.contains(currentEditor.GetHash()))
        editors.at(currentEditor.GetHash()).Get().Exit();
    currentEditor = InType;
    auto& c = editors.at(currentEditor.GetHash()).Get(); 
    c.Enter();
    c.GetEditor().GetMenu().SetSelected(GetCurrentName());
}

void RoomSubEditorManager::SetCurrent(const String &InName)
{
    for (auto& editor : editors)
        if (editor.second.Get().GetObjName() == InName)
            SetCurrent(Type(editor.first));
}

String RoomSubEditorManager::GetCurrentName() const
{
    return editors.at(GetCurrent().GetHash()).Get().GetObjName();
}

bool RoomSubEditorManager::IgnoreSave(const ECS::EntityID InID)
{
    for (auto& e : editors)
        if (e.second.Get().IgnoreSave(InID))
            return true;
    return false;
}
