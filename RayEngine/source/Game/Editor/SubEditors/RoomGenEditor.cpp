#include "RoomGenEditor.h"

#include "ECS/Manager.h"
#include "ECS/RoomConnection.h"
#include "ECS/Volume/CubeVolume.h"
#include "Editor/RoomSubEditorManager.h"
#include "UI/Builder.h"
#include "UI/Elements/RectImage.h"

void RoomGenEditor::Init()
{
    // Create UI
    UI::Builder builder = UI::Builder()
        .Push(UI::Container(UI::Transform::FromRect(Vec2F(80.0f, 80.0f), 0.0, 0.5f)))
            .Add(UI::RectImage(ResTexture("UI/T_Rectangle.png"), UI::Margin(5.0f)))
            .Add(UI::Image(ResTexture("UI/T_Exit.png"), UI::Transform::SimpleAnchor(0.5f)), "Loading");
    ui = builder.Build();
 
}

void RoomGenEditor::Deinit()
{
    startEntity = ECS::INVALID_ID;
    endEntity = ECS::INVALID_ID;
}

void RoomGenEditor::Enter()
{
    seed = Utility::Hash(Utility::Time::Get().Total());
        
    // Find start and end
    auto& sys = ECS::Manager::Get().GetSystem<SysRoomConnection>();
    for (const ECS::EntityID connection : sys.GetEntities())
    {
        if (sys.Get<RoomConnection>(connection).IsEnd)
            endEntity = connection;
        else
            startEntity = connection;
    }

    // Clear current volume
    auto& v = GetVolume();
    v.data.data.clear();
    v.UpdateCache(Mat4F());
}

void RoomGenEditor::Update()
{
    PROFILE();

    // Maybe step

    
}

void RoomGenEditor::Frame()
{
    return;
    auto& image = ui.Get<UI::Image>("Loading");
    UI::Transform trans = image.GetTransform();
    trans.rotation = Utility::Time::Get().Total();
    image.SetTransform(trans);
    ui.Draw();
}
