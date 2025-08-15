#include "RoomObjectEditor.h"

#include "ECS/Volume/CubeVolume.h"
#include "Editor/RoomEditor.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Attributes.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Instance.h"
#include "Utility/History/History.h"

void RoomHoverMenu::Init(RoomObjectEditorConfig &InConfig)
{
    for (auto& obj : InConfig.ObjectTypes.Get())
    {
        auto& entry = objectTypes.emplace_back();
        entry.name = obj.first;
        auto bp = obj.second.Get();
        CHECK_RETURN_LOG(!bp, "Failed to load " + obj.second.Identifier().Str());
        entry.id = bp->Instantiate();
    }
}

void RoomHoverMenu::Deinit()
{
    for (auto& obj : objectTypes)
        if (obj.id != ECS::INVALID_ID)
            ECS::Manager::Get().DestroyEntity(obj.id);
    objectTypes.clear();
}

void RoomHoverMenu::Update(const EditorCamera& InCamera)
{
    wheelRot = sin(Utility::Time::Get().Total());
    objRot = Utility::Time::Get().Total() * 0.5;
    
    auto& ecs = ECS::Manager::Get();
    for (int i = 0; i < static_cast<int>(objectTypes.size()); i++)
    {
        auto& obj = objectTypes[i];
        CHECK_CONTINUE(obj.id == ECS::INVALID_ID);
        ECS::Transform *t = ecs.GetComponent<ECS::Transform>(obj.id);
        CHECK_CONTINUE(!t);
        t->SetWorld(GetTrans(InCamera, i));
    }
}

String RoomHoverMenu::TryPick(const EditorCamera& InCamera, float InDotThreshold)
{
    // Else is picking placed objects?
    auto& ecs = ECS::Manager::Get();
    Vec3F mouseDir = InCamera.GetMouseDirection();
    Vec3F camPos = InCamera.GetPosition();
    
    float minDot = 0.0f;
    int index = -1;
    for (int i = 0; i < static_cast<int>(objectTypes.size()); ++i)
    {
        auto& o = objectTypes.at(i);
        ECS::Transform *t = ecs.GetComponent<ECS::Transform>(o.id);
        CHECK_CONTINUE(!t);
        Vec3F pos = t->GetPosition();
        
        Vec3F objDir = (pos - camPos).GetNormalized();
        float dot = Vec3F::Dot(mouseDir, objDir);
        if (dot > InDotThreshold && dot > minDot)
        {
            minDot = dot;
            index = i;
        }
    }

    CHECK_RETURN(index == -1, "");
    return objectTypes.at(index).name;
}

Mat4F RoomHoverMenu::GetTrans(const EditorCamera& InCamera, int InIndex)
{
    const float angle_step = (PI_FLOAT * 2 / objectTypes.size()); 
    const float offset = PI_FLOAT / 4;
    const float angle = angle_step * InIndex + offset + wheelRot;
    const Vec3F worldDir = InCamera.ClipToWorld(screenPos);
    const Vec3F world = InCamera.GetPosition() + worldDir * depth;
    const Vec2F wheelPos = Vec2F(cos(angle), sin(angle)) * wheelScale;
    const Mat4F camRot = Mat4F::FromEuler(InCamera.GetRotation());
    const Vec3F objPos = world + wheelPos.x * camRot.Right() + wheelPos.y * camRot.Up();
    const QuatF rot = QuatF::FromDirection(InCamera.GetRotation()) * QuatF::FromEuler({ 0, objRot, 0 });
    return Mat4F(objPos, rot, scale);
}

void RoomObjectEditor::Init()
{
    config.LoadConfig();

}

void RoomObjectEditor::Deinit()
{
    config.SaveConfig(); 
}

void RoomObjectEditor::Update()
{
    CHECK_RETURN(!IsCurrent());

    auto& v = GetVolume();
    auto& cam = GetEditor().GetEditorCamera();
    Vec3F mouseDir = cam.GetMouseDirection();
    Vec3F camPos = cam.GetPosition();
    
    CHECK_RETURN(GetEditor().IsFreecam())
    hoverMenu.Update(cam);
    
    
    // Clicks
    // On object - select object and drag
    // On miniature - create object and drag

    if (Input::Action::Get("LM").Pressed())
    {
        // Is hovering miniature?
        hoverMenu.TryPick(cam, config.DotThreshold);

        // Else is picking placed objects?
        float minDot = 0.0f;
        ECS::VolumeCoordKey coord = 0;
        for (auto& o : loadedObjects)
        {
            Vec3F pos = v.CoordToPos(o.first);
            Vec3F objDir = (pos - camPos).GetNormalized();
            float dot = Vec3F::Dot(mouseDir, objDir);
            if (dot > config.DotThreshold && dot > minDot)
            {
                minDot = dot;
                coord = o.first;
            }
        }

        // Begin dragging the obj!
        if (coord != 0)
        {
            
        }
    }
    
    // Move object using trace 
     

    // Rotate object!
    //if (Input::Action::Get("Rotate").Pressed())

}

void RoomObjectEditor::Frame()
{
}

void RoomObjectEditor::Enter()
{
    LoadRoom();
    hoverMenu.Init(config);
}

void RoomObjectEditor::Exit()
{
    DestroyLoaded();
    hoverMenu.Deinit();
}

ECS::EntityID RoomObjectEditor::GetObject(const RoomObject &InObj, bool InSnap)
{
    auto& obj = loadedObjects[InObj.Coord];
    if (obj.id == ECS::INVALID_ID)
        return LoadObject(InObj);
    
    if (InSnap)
    {
        Vec3F pos = GetVolume().CoordToPos(InObj.Coord.Get());
        float angle = InObj.Rotations.Get() * PI_FLOAT / 4.0f;
        QuatF rot = QuatF::FromEuler({0, angle, 0});
        if (auto trans = ECS::Manager::Get().GetComponent<ECS::Transform>(obj.id))
            trans->SetWorld(Mat4F(pos, rot, Vec3F::One()));
    }
    return obj.id;    
}

ECS::EntityID RoomObjectEditor::LoadObject(const RoomObject &InObj) const
{
    auto& objType = config.ObjectTypes.Get().at(InObj.Object);
    auto bp = objType.Get();
    CHECK_RETURN_LOG(!bp, "Unknown object type: " + InObj.Object.Get(), ECS::INVALID_ID);

    Vec3F pos = GetVolume().CoordToPos(InObj.Coord.Get());
    float angle = InObj.Rotations.Get() * PI_FLOAT / 4.0f;
    QuatF rot = QuatF::FromEuler({0, angle, 0});
    return bp->Instantiate(Mat4F(pos, rot, Vec3F::One()));
}

void RoomObjectEditor::LoadRoom()
{
    // TODO: Diff
    DestroyLoaded(); 
    for (auto& o : GetRoom().Objects.Get())
    {
        o.second.Coord = o.first;
        GetObject(o.second, true);
    }
}

void RoomObjectEditor::DestroyLoaded()
{
    for (auto& o : loadedObjects)
        ECS::Manager::Get().DestroyEntity(o.second.id);
    loadedObjects.clear();
}




