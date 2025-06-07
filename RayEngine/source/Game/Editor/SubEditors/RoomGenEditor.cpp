#include "RoomGenEditor.h"

#include "ECS/Manager.h"
#include "ECS/RoomConnection.h"
#include "ECS/Volume/CubeVolume.h"
#include "Editor/RoomSubEditorManager.h"
#include "Rooms/Generation/RoomGenBase.h"
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

    pathGen.SetOwner(this); 
    volumeGen.SetOwner(this); 
    propsGen.SetOwner(this); 
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
    v.data.clear();
    v.UpdateCache(Mat4F());

    // Maybe clear spawned objects too?
    
    // Clear gen data
    pathGen.Clear();
    volumeGen.Clear();
    propsGen.Clear();
    SetStage(GenerationStage::PATH);
}

RoomGenBase* RoomGenEditor::GetCurrentStage()
{
    switch (currentStage)
    {
    case GenerationStage::PATH:
        return &pathGen;
    case GenerationStage::VOLUME:
        return &volumeGen;
    case GenerationStage::PROPPING:
        return &propsGen;
    case GenerationStage::FINISHED:
    default:
            return nullptr;
    }
}

void RoomGenEditor::SetStage(const GenerationStage InStage)
{
    currentStage = InStage;
    if (RoomGenBase* s = GetCurrentStage())
        s->Init();
}

void RoomGenEditor::Update()
{
    if (RoomGenBase* stage = GetCurrentStage())
        if (stage->Step())
            SetStage(static_cast<GenerationStage>(static_cast<uint8>(currentStage) + 1));
    if (currentStage == GenerationStage::FINISHED)
        owner->SetMode(SubEditorMode::OBJECTS);
}

void RoomGenEditor::Frame(bool InIsCameraControlling)
{
    //UI::Image& image = UI->Get<UI::Image>("Loading");
    //UI::Transform trans = image.GetTransform();
    //trans.Rotation = Utility::Time::Get().Total();
    //image.SetTransform(trans);
    
    //UI->Draw();
}
