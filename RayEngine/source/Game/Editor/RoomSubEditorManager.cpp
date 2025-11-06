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
#include "SubEditors/RoomPreviewEditor.h"

#define CREATE_SUBEDITOR(x, name) \
auto CONCAT(hash,x) = Type::GetHash<x>(); \
editors[CONCAT(hash,x)] = x(); \
auto& CONCAT(obj,x) = editors.at(CONCAT(hash,x)).Get(); \
CONCAT(obj,x).editor = InEditor; \
CONCAT(obj,x).option = name;

void RoomSubEditorManager::Init(RoomEditor* InEditor)
{
    CHECK_ASSERT(!InEditor, "Invalid editor")
    
    CREATE_SUBEDITOR(RoomConnectionEditor, "Connection");
    CREATE_SUBEDITOR(RoomPathEditor, "Path");
    CREATE_SUBEDITOR(RoomGenEditor, "Generate");
    CREATE_SUBEDITOR(RoomVolumeEditor, "Block");
    CREATE_SUBEDITOR(RoomObjectEditor, "Objects");
    CREATE_SUBEDITOR(RoomPreviewEditor, "Preview");

    roomType = static_cast<RoomType>(InEditor->GetRoom().Info.Get().Type.Get());
    auto& menu = InEditor->GetMenu();
    for (auto& editor : editors)
    {
        auto& e = editor.second.Get(); 
        if (e.IsEnabled(roomType))
            menu.AddOption(e.GetObjName(), e.option);
    }
        
    Get<RoomVolumeEditor>().Create();
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
        if (e.second.Get().IsEnabled(roomType))
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

#ifdef IMGUI_ENABLE

void RoomSubEditorManager::DebugDraw()
{
    auto hash = GetCurrent().GetHash();
    CHECK_RETURN(!editors.contains(hash))
    String editorName = editors.at(hash).Get<>().GetObjName();
    ImGui::Text("Current room: %s", editorName.c_str());
    for (auto& e : editors)
        e.second.Get().DebugDraw();
}

#endif

void RoomSubEditorManager::SetCurrent(const Type &InType)
{
    CHECK_RETURN(currentEditor == InType);
    auto& c = editors.at(InType.GetHash()).Get();
    CHECK_RETURN(!c.IsEnabled(roomType));
    if (editors.contains(currentEditor.GetHash()))
        editors.at(currentEditor.GetHash()).Get().Exit();
    currentEditor = InType; 
    c.Enter();
    c.GetEditor().GetMenu().SetSelected(GetCurrentName());
}

void RoomSubEditorManager::SetCurrent(const String &InName)
{
    for (auto& editor : editors)
        if (editor.second.Get().IsEnabled(roomType))
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
