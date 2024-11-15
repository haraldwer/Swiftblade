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
    UI = builder.Build();

    PathGen.SetOwner(this); 
    VolumeGen.SetOwner(this); 
    PropsGen.SetOwner(this); 
}

void RoomGenEditor::Deinit()
{
    StartEntity = ECS::InvalidID;
    EndEntity = ECS::InvalidID;
}

void RoomGenEditor::Enter()
{
    Seed = Utility::Hash(Utility::Time::Get().Total());
        
    // Find start and end
    auto& sys = ECS::Manager::Get().GetSystem<SysRoomConnection>();
    for (const ECS::EntityID connection : sys.GetEntities())
    {
        if (sys.Get<RoomConnection>(connection).IsEnd)
            EndEntity = connection;
        else
            StartEntity = connection;
    }

    // Clear current volume
    auto& v = GetVolume();
    v.Data.clear();
    v.UpdateCache(Mat4F());

    // Maybe clear spawned objects too?
    
    // Clear gen data
    PathGen.Clear();
    VolumeGen.Clear();
    PropsGen.Clear();
    SetStage(GenerationStage::PATH);
}

RoomGenBase* RoomGenEditor::GetCurrentStage()
{
    switch (CurrentStage)
    {
    case GenerationStage::PATH:
        return &PathGen;
    case GenerationStage::VOLUME:
        return &VolumeGen;
    case GenerationStage::PROPPING:
        return &PropsGen;
    }
    return nullptr;
}

void RoomGenEditor::SetStage(const GenerationStage InStage)
{
    CurrentStage = InStage;
    if (RoomGenBase* s = GetCurrentStage())
        s->Init();
}

void RoomGenEditor::Update()
{
    if (RoomGenBase* stage = GetCurrentStage())
        if (stage->Step())
            SetStage(static_cast<GenerationStage>(static_cast<uint8>(CurrentStage) + 1));
    if (CurrentStage == GenerationStage::FINISHED)
        Owner->SetMode(SubEditorMode::OBJECTS);
}

void RoomGenEditor::Frame(bool InIsCameraControlling)
{
    if (UI)
    {
        //UI::Image& image = UI->Get<UI::Image>("Loading");
        //UI::Transform trans = image.GetTransform();
        //trans.Rotation = Utility::Time::Get().Total();
        //image.SetTransform(trans);
        
        //UI->Draw();
    }
}
