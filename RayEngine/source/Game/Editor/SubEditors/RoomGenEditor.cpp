#include "RoomGenEditor.h"

#include "ECS/Manager.h"
#include "ECS/RoomConnection.h"
#include "ECS/Volume/CubeVolume.h"
#include "Editor/RoomEditor.h"
#include "Editor/RoomSubEditorManager.h"
#include "../EditRoom.h"
#include "Input/Action.h"
#include "UI/Builder.h"
#include "UI/Elements/RectImage.h"

void RoomGenEditor::Init()
{
}

void RoomGenEditor::Deinit()
{
}

void RoomGenEditor::Enter()
{
    auto& data = GetRoom().volumeData;
    if (data.data.empty())
        StartGen();
}

void RoomGenEditor::Update()
{
    CHECK_RETURN(!IsCurrent())
    CHECK_RETURN(!GetEditor().CanEdit())
    
    if (Input::Action::Get("Ctrl").Down())
        if (Input::Action::Get("Generate", "RoomEditor").Pressed())
            StartGen();

    CHECK_RETURN(!generating);
    
    auto& v = GetVolume();
    auto& data = GetRoom().volumeData;
    generating = volumeGenerator.Step(data);
    if (generating)
    {
        // Copy data
        v.data = data;
        v.UpdateCache(Mat4F());
    }
}

void RoomGenEditor::StartGen()
{
    auto& v = GetVolume();
    auto& data = GetRoom().volumeData;
    auto& path = GetRoom().Path.Get();
    volumeGenerator = RoomGenVolume(path);
    generating = true;
            
    // Clear current volume
    data.data.clear(); 
    v.data.data.clear();
    v.UpdateCache(Mat4F());
}
