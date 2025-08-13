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
    config.LoadConfig();
    
    LoadPlacedObjects();

    SetEditObject(1);
}

void RoomObjectEditor::Deinit()
{
    config.SaveConfig(); 
}

void RoomObjectEditor::Update()
{
    PROFILE();
    
    CHECK_RETURN(objectID == ECS::INVALID_ID)

    auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(objectID);
    CHECK_RETURN(!trans);

    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    
    // Move object using trace 
    targetPos = UpdateCameraTrace();
    if (targetPos != Vec3F::Zero())
    {
        const Vec3F currPos = trans->GetPosition();
        trans->SetPosition(Lerp(currPos, targetPos, 50.0f * dt));
    } 

    // Rotate object!
    if (Input::Action::Get("Rotate").Pressed())
        targetRot.y += PI_FLOAT * 0.25f;
    const QuatF rot = trans->GetRotation();
    trans->SetRotation(QuatF::Slerp(rot, QuatF::FromEuler(targetRot), 50.0f * dt));

    if (Input::Action::Get("LM").Pressed())
        PlaceObject();

    if (Input::Action::Get("RM").Pressed())
        RemoveObject();
}

void RoomObjectEditor::Frame()
{
}

void RoomObjectEditor::DebugDraw()
{
    ImGui::Text("Object editing mode");
    
    // TODO: Click on the object type you want to place
    
    auto transSys = ECS::Manager::Get().GetSystem<ECS::SysTransform>();
    const auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(objectID);
    CHECK_RETURN(!trans);
    
    transSys.EditValues(objectID);
    if (!InIsCameraControlling)
    {
        if (transSys.EditGizmo(objectID))
        {
            const auto& v = GetVolume();
            const Vec3F pos = trans->GetPosition();
            targetPos = v.CoordToPos(v.PosToCoord(pos, Mat4F()), Mat4F());  
            targetRot = trans->GetRotation().Euler();
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
    CHECK_RETURN_LOG(InIndex < 0 || InIndex >= static_cast<int>(config.Blueprints.Get().size()), "BP index outside of range");
    config.BPIndex = InIndex; 
    NewEditObject(); 
}

void RoomObjectEditor::PlaceObject()
{
    struct EditData
    {
        ObjectData prevObjectData = {};
        ObjectData newObjectData = {};
    } data;

    data.newObjectData = {
        targetPos,
        targetRot,
        config.BPIndex,
        ECS::INVALID_ID
    };
    data.prevObjectData = GetPlacedObjectData(GetKey(targetPos));
    data.prevObjectData.position = targetPos;
    data.prevObjectData.id = ECS::INVALID_ID; 
    
    GetHistory().AddChange(Utility::Change<EditData>(
        [&](const EditData& InData)
        {
            PlaceObject(InData.newObjectData);
        },
        [&](const EditData& InData)
        {
            PlaceObject(InData.prevObjectData);
        },
        data));
}

void RoomObjectEditor::RemoveObject()
{
    struct EditData
    {
        ObjectData prevObjectData = {};
        Vec3F position = {};
    } data;

    data.position = targetPos;
    data.prevObjectData = GetPlacedObjectData(GetKey(targetPos));
    data.prevObjectData.id = ECS::INVALID_ID; 
    
    GetHistory().AddChange(Utility::Change<EditData>(
        [&](const EditData& InData) { RemovePlacedObject(GetKey(InData.position)); },
        [&](const EditData& InData) { PlaceObject(InData.prevObjectData); },
        data));
}

void RoomObjectEditor::NewEditObject()
{
    DestroyEditObject();
    objectID = CreateObject(config.BPIndex, Mat4F(targetPos, QuatF::FromEuler(targetRot), Vec3F::One()));
}

void RoomObjectEditor::DestroyEditObject()
{
    if (objectID != ECS::INVALID_ID)
    {
        ECS::Manager::Get().DestroyEntity(objectID);
        objectID = ECS::INVALID_ID; 
    }
}

void RoomObjectEditor::LoadPlacedObjects()
{
    Map<String, int> blueprintMap;
    auto& bps = config.Blueprints.Get();
    for (size_t i = 0; i < bps.size(); i++)
        blueprintMap[bps[i].Identifier()] = i; 
    
    const auto entities = ECS::Manager::Get().GetAllEntities();
    for (const ECS::EntityID entity : entities)
    {
        CHECK_CONTINUE(entity == GetVolumeID());
        CHECK_CONTINUE(entity == objectID);
        
        const auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(entity);
        const auto* attr = ECS::Manager::Get().GetComponent<ECS::Attributes>(entity);
        CHECK_CONTINUE(!trans || !attr);

        // Try to find BP index
        String bpID = attr->blueprint.Identifier();
        auto bpFind = blueprintMap.find(bpID);
        CHECK_CONTINUE(bpFind == blueprintMap.end());
        
        ObjectData data;
        data.position = trans->GetPosition();
        data.rotation = trans->GetRotation().Euler();
        data.objectType = blueprintMap.at(bpID);
        data.id = entity; 
        
        PlaceObject(data);
    }
}

void RoomObjectEditor::PlaceObject(const ObjectData& InObjectData)
{
    const uint32 key = GetKey(InObjectData.position);
    RemovePlacedObject(key);
    
    auto& entry = placedObjects[key];
    entry = InObjectData;
    if (entry.id == ECS::INVALID_ID && InObjectData.objectType != -1)
        entry.id = CreateObject(entry.objectType, Mat4F(entry.position, QuatF::FromEuler(entry.rotation), Vec3F::One()));
}

void RoomObjectEditor::RemovePlacedObject(const uint32 InKey)
{
    auto find = placedObjects.find(InKey);
    if (find != placedObjects.end())
    {
        // Destroy object and remove entry
        if (find->second.id != ECS::INVALID_ID)
            ECS::Manager::Get().DestroyEntity(find->second.id);
        placedObjects.erase(InKey);
    }
}

uint32 RoomObjectEditor::GetKey(const Vec3F& InPos) const
{
    return GetVolume().PosToCoord(InPos, Mat4F()).key; 
}

RoomObjectEditor::ObjectData RoomObjectEditor::GetPlacedObjectData(const uint32 InKey) const
{
    const auto find = placedObjects.find(InKey);
    if (find != placedObjects.end())
        return find->second;
    return {};
}

ECS::EntityID RoomObjectEditor::CreateObject(const int InIndex, const Mat4F& InMat) const
{
    CHECK_RETURN_LOG(InIndex < 0 || InIndex >= static_cast<int>(config.Blueprints.Get().size()), "BP index outside of range", ECS::INVALID_ID);
    if (const auto bp = config.Blueprints.Get()[InIndex].Get())
    {
        const auto id = bp->Instantiate(InMat);
        if (auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(id))
            trans->SetWorld(InMat); 
        return id;
    }
    return ECS::INVALID_ID;
}
