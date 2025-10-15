#include "RoomObjectEditor.h"

#include "ECS/Volume/CubeVolume.h"
#include "Editor/RoomEditor.h"
#include "Editor/Menus/MenuRoomObjects.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Attributes.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Instance.h"
#include "Utility/History/History.h"

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
    CHECK_RETURN(!GetEditor().CanEdit())
    
    if (Input::Action::Get("LM").Pressed())
    {
        TryPickObject();
        if (placeObj.Coord != 0 && placeID != ECS::INVALID_ID &&
            Input::Action::Get("Ctrl").Down())
            RemoveObject();
    }
    
    if (placeObj.Coord != 0 && placeID != ECS::INVALID_ID)
    {
        if (Input::Action::Get("LM").Down())
            MovePlaceObject();
        if (Input::Action::Get("LM").Released())
            PlaceObject(); 
    }
    
    UpdateTransforms();
}

void RoomObjectEditor::TryPickObject()
{
    CHECK_ASSERT(!menu, "Invalid menu");
    
    auto& objects = GetRoom().Objects.Get();
    auto& v = GetVolume();
    auto& cam = GetEditor().GetEditorCamera();
    Vec3F mouseDir = cam.GetMouseDirection();
    Vec3F camPos = cam.GetPosition();
    
    // Is hovering miniature?
    String pick = menu->TryPick();
    if (!pick.empty() && pick != "")
    {
        LOG("Pick!");
        CHECK_RETURN_LOG(!config.ObjectTypes.Get().contains(pick), "Unknown object type when picked: " + pick);
        
        // Create new!
        EditRoomObject obj;
        obj.Coord = CameraTrace(8).key;
        obj.Object = pick;
        placeObj= obj;
        placeID = CreateObject(obj);
        return;
    }

    // Else is picking placed objects?
    float minDot = 0.0f;
    ECS::VolumeCoordKey coord = 0;
    for (auto& o : objects)
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

    if (coord != 0)
    {
        // Set place object
        placeObj = objects[coord];
        if (loadedObjects.contains(coord))
            placeID = loadedObjects.at(coord);
        else placeID = CreateObject(placeObj);

        // Remove from everywhere!
        objects.erase(coord);
        loadedObjects.erase(coord);
        movedFrom = coord;
        return;
    }

    // Reset!
    placeObj = EditRoomObject();
    placeID = ECS::INVALID_ID;
}

void RoomObjectEditor::MovePlaceObject()
{
    if (Input::Action::Get("Rotate").Pressed())
        placeObj.Rotations.Get()++;

    // Move differently if holding something? 
    auto& v = GetVolume();
    placeObj.Coord = v.PosToCoord(DragMove(v.CoordToPos(placeObj.Coord.Get()))).key;
}

void RoomObjectEditor::PlaceObject()
{
    auto& objects = GetRoom().Objects.Get();
    const struct RoomObjChange
    {
        EditRoomObject prevObj;
        EditRoomObject newObj;
        ECS::VolumeCoordKey coord;
        ECS::VolumeCoordKey from;
    } change {
        objects.contains(placeObj.Coord) ? objects.at(placeObj.Coord) : EditRoomObject(),
        placeObj,
        placeObj.Coord,
        movedFrom.key
    };
    
    GetHistory().AddChange(Utility::Change<RoomObjChange>(
        [&](const RoomObjChange& InData)
        {
            // Destroy existing object
            RemoveLoaded(InData.coord);

            // Set object
            auto& localObjects = GetRoom().Objects.Get();
            localObjects[InData.coord] = InData.newObj;

            // Handle instance
            if (InData.newObj == placeObj) // We are sure that the newly placed object is the same as this history object 
                loadedObjects[InData.coord] = placeID;
            else
                LoadObject(InData.newObj);

            // Clear where it was moved from
            if (InData.from != 0)
            {
                localObjects.erase(InData.from);
                RemoveLoaded(InData.from);
            }
        },
        [&](const RoomObjChange& InData)
        {
            // Maybe destroy existing object
            RemoveLoaded(InData.coord);

            // Reset previous object
            auto& localObjects = GetRoom().Objects.Get();
            if (InData.prevObj.Coord == 0)
            {
                localObjects.erase(InData.coord);
            }
            else
            {
                // Set to what was there before
                localObjects[InData.coord] = InData.prevObj;
                LoadObject(InData.prevObj);
            }

            // Move this back to original position
            if (InData.from != 0)
            {
                auto orgObj = InData.newObj;
                orgObj.Coord.Get() = InData.from;
                localObjects[InData.from] = orgObj;
                RemoveLoaded(InData.from);
                LoadObject(orgObj);
            }
        },
        change));

    placeID = ECS::INVALID_ID;
    placeObj = {};
    movedFrom = {};
}

void RoomObjectEditor::RemoveObject()
{
    const struct RoomObjRemove
    {
        EditRoomObject obj;
        ECS::VolumeCoordKey from;
    } change {
        placeObj,
        movedFrom.key
    };
    
    GetHistory().AddChange(Utility::Change<RoomObjRemove>(
    [&](const RoomObjRemove& InData)
        {
            RemoveLoaded(InData.from);
            auto& objects = GetRoom().Objects.Get();
            objects.erase(InData.from);
        },
        [&](const RoomObjRemove& InData)
        {
            // Clear whats there now
            RemoveLoaded(InData.from);

            // And set the data
            EditRoomObject obj = InData.obj;
            obj.Coord = InData.from;
            auto& objects = GetRoom().Objects.Get();
            objects[InData.from] = obj;
            LoadObject(obj);
            
        },
        change));
    
    ECS::Manager::Get().DestroyEntity(placeID);
    
    placeObj = EditRoomObject();
    placeID = ECS::INVALID_ID;
    movedFrom = {};
}

void RoomObjectEditor::UpdateTransforms()
{
    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysTransform>();
    auto& objects = GetRoom().Objects.Get();
    for (auto& o : objects)
        if (loadedObjects.contains(o.first))
            UpdateTransform(sys, loadedObjects.at(o.first), o.second);
    if (placeObj.Coord != 0 && placeID != ECS::INVALID_ID)
        UpdateTransform(sys, placeID, placeObj);
}

void RoomObjectEditor::UpdateTransform(ECS::SysTransform &InSys, ECS::EntityID InID, const EditRoomObject &InObj)
{
    auto& t = InSys.Get(InID);
    Mat4F current = t.World();
    Mat4F target = GetTrans(InObj);
    Mat4F world = Mat4F::Lerp(current, target, config.LerpSpeed.Get());
    InSys.Get(InID).SetWorld(world);
}

Mat4F RoomObjectEditor::GetTrans(const EditRoomObject &InObj) const
{
    Vec3F pos = GetVolume().CoordToPos(InObj.Coord.Get());
    float angle = InObj.Rotations.Get() * PI_FLOAT / 4.0f;
    QuatF rot = QuatF::FromEuler({0, angle, 0});
    return Mat4F(pos, rot, Vec3F::One());
}

void RoomObjectEditor::Frame()
{
    
}

void RoomObjectEditor::Enter()
{
    LoadRoom();
    CHECK_ASSERT(menu, "Menu already set");
    menu = Menu::Manager::Get().Push<MenuRoomObjects>();
    menu->SetConfig(config);
}

void RoomObjectEditor::Exit()
{
    DestroyLoaded();
    CHECK_ASSERT(!menu, "Menu was null");
    Menu::Manager::Get().Close(menu);
    menu = nullptr;
}

ECS::EntityID RoomObjectEditor::LoadObject(const EditRoomObject &InObj)
{
    bool contains = loadedObjects.contains(InObj.Coord);
    auto& obj = loadedObjects[InObj.Coord];
    if (!contains)
    {
        obj = CreateObject(InObj);
    }
    else if (auto trans = ECS::Manager::Get().GetComponent<ECS::Transform>(obj))
        trans->SetWorld(GetTrans(InObj));
    return obj;
}

ECS::EntityID RoomObjectEditor::CreateObject(const EditRoomObject &InObj) const
{
    auto& objType = config.ObjectTypes.Get().at(InObj.Object);
    auto bp = objType.Get();
    CHECK_RETURN_LOG(!bp, "Unknown object type: " + InObj.Object.Get(), ECS::INVALID_ID);
    return bp->Instantiate(GetTrans(InObj));
}

void RoomObjectEditor::LoadRoom()
{
    // TODO: Diff
    DestroyLoaded();
    for (auto& o : GetRoom().Objects.Get())
    {
        o.second.Coord = o.first;
        LoadObject(o.second);
    }
}

void RoomObjectEditor::DestroyLoaded()
{
    for (auto& o : loadedObjects)
        ECS::Manager::Get().DestroyEntity(o.second);
    loadedObjects.clear();
}

void RoomObjectEditor::RemoveLoaded(ECS::VolumeCoordKey InKey)
{
    if (loadedObjects.contains(InKey))
    {
        const ECS::EntityID id = loadedObjects.at(InKey);
        ECS::Manager::Get().DestroyEntity(id);
        loadedObjects.erase(InKey);
    }
}
