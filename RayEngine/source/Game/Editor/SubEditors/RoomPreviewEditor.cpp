#include "RoomPreviewEditor.h"

#include "Editor/Menus/MenuRoomPreview.h"
#include "Menu/Manager.h"

void RoomPreviewEditor::Init()
{
    RoomSubEditor::Init();
}

void RoomPreviewEditor::Deinit()
{
    RoomSubEditor::Deinit();
}

void RoomPreviewEditor::Enter()
{
    RoomSubEditor::Enter();
    Menu::Manager::Get().Push<MenuRoomPreview>();
}

void RoomPreviewEditor::Exit()
{
    RoomSubEditor::Exit();
    Menu::Manager::Get().Pop();
}
