#include "RoomObjectEditor.h"

#include "ECS/Volume/CubeVolume.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Attributes.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Instance.h"
#include "ImGui/imgui.h"
#include "Utility/History/History.h"

void RoomObjectEditor::Init()
{
    Config.LoadConfig();
    
    LoadPlacedObjects();

    SetEditObject(1);
}

void RoomObjectEditor::Deinit()
{
    Config.SaveConfig(); 
}

void RoomObjectEditor::Update()
{
    CHECK_RETURN(ObjectID == ECS::InvalidID)

    auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(ObjectID);
    CHECK_RETURN(!trans);

    float dt = static_cast<float>(Utility::Time::Get().Delta());
    
    // Move object using trace 
    TargetPos = UpdateCameraTrace();
    if (TargetPos != Vec3F::Zero())
    {
        const Vec3F currPos = trans->GetPosition();
        trans->SetPosition(Lerp(currPos, TargetPos, 50.0f * dt));
    } 

    // Rotate object!
    if (Input::Action::Get("Rotate").Pressed())
        TargetRot.y += PI_FLOAT * 0.25f;
    const QuatF rot = trans->GetRotation();
    trans->SetRotation(QuatF::Slerp(rot, QuatF::FromEuler(TargetRot), 50.0f * dt));

    if (Input::Action::Get("LM").Pressed())
        PlaceObject();

    if (Input::Action::Get("RM").Pressed())
        RemoveObject();
}

void RoomObjectEditor::Frame(bool InIsCameraControlling)
{
}

void RoomObjectEditor::DebugDraw(bool InIsCameraControlling)
{
    ImGui::Text("Object editing mode");
    
    // TODO: Click on the object type you want to place
    
    auto transSys = ECS::Manager::Get().GetSystem<ECS::SysTransform>();
    auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(ObjectID);
    CHECK_RETURN(!trans);
    
    transSys.EditValues(ObjectID);
    if (!InIsCameraControlling)
    {
        if (transSys.EditGizmo(ObjectID))
        {
            auto& v = GetVolume();
            const Vec3F pos = trans->GetPosition();
            TargetPos = v.CoordToPos(v.PosToCoord(pos, Mat4F()), Mat4F());  
            TargetRot = trans->GetRotation().Euler();
        }
    }
    
    if (ImGui::Button("Place"))
        PlaceObject();
}

void RoomObjectEditor::Enter()
{
    NewEditObject();
}

void RoomObjectEditor::Exit()
{
    DestroyEditObject();
}

void RoomObjectEditor::SetEditObject(const int InIndex)
{
    CHECK_RETURN_LOG(InIndex < 0 || InIndex >= Config.Blueprints.Get().size(), "BP index outside of range");
    Config.BPIndex = InIndex; 
    NewEditObject(); 
}

void RoomObjectEditor::PlaceObject()
{
    struct EditData
    {
        ObjectData PrevObjectData;
        ObjectData NewObjectData;
    } data;

    data.NewObjectData = {
        TargetPos,
        TargetRot,
        Config.BPIndex,
        ECS::InvalidID
    };
    data.PrevObjectData = GetPlacedObjectData(GetKey(TargetPos));
    data.PrevObjectData.Position = TargetPos;
    data.PrevObjectData.ID = ECS::InvalidID; 
    
    GetHistory().AddChange(Utility::Change<EditData>(
        [&](const EditData& InData)
        {
            PlaceObject(InData.NewObjectData);
        },
        [&](const EditData& InData)
        {
            PlaceObject(InData.PrevObjectData);
        },
        data));
}

void RoomObjectEditor::RemoveObject()
{
    struct EditData
    {
        ObjectData PrevObjectData;
        Vec3F Position;
    } data;

    data.Position = TargetPos;
    data.PrevObjectData = GetPlacedObjectData(GetKey(TargetPos));
    data.PrevObjectData.ID = ECS::InvalidID; 
    
    GetHistory().AddChange(Utility::Change<EditData>(
        [&](const EditData& InData) { RemovePlacedObject(GetKey(InData.Position)); },
        [&](const EditData& InData) { PlaceObject(InData.PrevObjectData); },
        data));
}

void RoomObjectEditor::NewEditObject()
{
    DestroyEditObject();
    ObjectID = CreateObject(Config.BPIndex, Mat4F(TargetPos, QuatF::FromEuler(TargetRot), Vec3F::One()));
}

void RoomObjectEditor::DestroyEditObject()
{
    if (ObjectID != ECS::InvalidID)
    {
        ECS::Manager::Get().DestroyEntity(ObjectID);
        ObjectID = ECS::InvalidID; 
    }
}

void RoomObjectEditor::LoadPlacedObjects()
{
    Map<String, int> blueprintMap;
    auto& bps = Config.Blueprints.Get();
    for (int i = 0; i < bps.size(); i++)
        blueprintMap[bps[i].Identifier()] = i; 
    
    const auto entities = ECS::Manager::Get().GetAllEntities();
    for (const ECS::EntityID entity : entities)
    {
        CHECK_CONTINUE(entity == GetVolumeID());
        CHECK_CONTINUE(entity == ObjectID);
        
        const auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(entity);
        const auto* attr = ECS::Manager::Get().GetComponent<ECS::Attributes>(entity);
        CHECK_CONTINUE(!trans || !attr);

        // Try to find BP index
        String bpID = attr->Blueprint.Identifier();
        auto bpFind = blueprintMap.find(bpID);
        CHECK_CONTINUE(bpFind == blueprintMap.end());
        
        ObjectData data;
        data.Position = trans->GetPosition();
        data.Rotation = trans->GetRotation().Euler();
        data.ObjectType = blueprintMap.at(bpID);
        data.ID = entity; 
        
        PlaceObject(data);
    }
}

void RoomObjectEditor::PlaceObject(const ObjectData& InObjectData)
{
    const uint32 key = GetKey(InObjectData.Position);
    RemovePlacedObject(key);
    
    auto& entry = PlacedObjects[key];
    entry = InObjectData;
    if (entry.ID == ECS::InvalidID && InObjectData.ObjectType != -1)
        entry.ID = CreateObject(entry.ObjectType, Mat4F(entry.Position, QuatF::FromEuler(entry.Rotation), Vec3F::One()));
}

void RoomObjectEditor::RemovePlacedObject(const uint32 InKey)
{
    auto find = PlacedObjects.find(InKey);
    if (find != PlacedObjects.end())
    {
        // Destroy object and remove entry
        if (find->second.ID != ECS::InvalidID)
            ECS::Manager::Get().DestroyEntity(find->second.ID);
        PlacedObjects.erase(InKey);
    }
}

uint32 RoomObjectEditor::GetKey(const Vec3F& InPos) const
{
    return GetVolume().PosToCoord(InPos, Mat4F()).Key; 
}

RoomObjectEditor::ObjectData RoomObjectEditor::GetPlacedObjectData(const uint32 InKey) const
{
    const auto find = PlacedObjects.find(InKey);
    if (find != PlacedObjects.end())
        return find->second;
    return {};
}

ECS::EntityID RoomObjectEditor::CreateObject(const int InIndex, const Mat4F& InMat) const
{
    CHECK_RETURN_LOG(InIndex < 0 || InIndex >= Config.Blueprints.Get().size(), "BP index outside of range", ECS::InvalidID);
    if (const auto bp = Config.Blueprints.Get()[InIndex].Get())
    {
        const auto id = bp->Instantiate(InMat);
        if (auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(id))
            trans->SetWorld(InMat); 
        return id;
    }
    return ECS::InvalidID;
}
