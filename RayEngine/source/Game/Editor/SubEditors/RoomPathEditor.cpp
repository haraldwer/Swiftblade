#include "RoomPathEditor.h"

#include "RoomConnectionEditor.h"
#include "ECS/Volume/CubeVolume.h"
#include "Editor/RoomEditor.h"
#include "Editor/Room/Room.h"
#include "Generation/RoomGenPath.h"
#include "Input/Action.h"

void RoomPathEditor::Init()
{
    config.LoadConfig();

    VerifyPath();
    selectedIndex = -1;

    pointID = MeshInstance::GenPersistentID();
    linkID = MeshInstance::GenPersistentID();
    pointHash = MeshInstance::GenHash(config.PathPoint.Get(), config.PathMaterial.Get());
    linkHash = MeshInstance::GenHash(config.PathLink.Get(), config.PathMaterial.Get());
}

void RoomPathEditor::Deinit()
{
    auto& meshes = GetEditor().GetRenderScene().Meshes();
    meshes.Remove(pointHash, pointID);
    meshes.Remove(linkHash, linkID);
    config.SaveConfig();
}

void RoomPathEditor::Update()
{
    CHECK_RETURN(GetCurrent() == Type::Get<RoomConnectionEditor>());
    VerifyPath();

    CHECK_RETURN(!IsCurrent())
    CHECK_RETURN(!GetEditor().CanEdit())
    
    auto& path = GetRoom().Path.Get();
    SmoothPath();
    
    // Select
    // Move
    // Remove 
    // Add
    // Generate
    // Clear all

    struct RoomPathChange
    {
        Vector<ECS::VolumeCoordKey> newPath;
        Vector<ECS::VolumeCoordKey> prevPath;
    };

    auto addChange = [&](const Vector<ECS::VolumeCoordKey> &InPrev) {
        GetHistory().AddChange(Utility::Change<RoomPathChange>(
            [&](const RoomPathChange& InData)
            {
                GetRoom().Path = InData.newPath;
                    
            },
            [&](const RoomPathChange& InData)
            {
                GetRoom().Path = InData.prevPath;
                    
            },
            {
                path,
                InPrev
            }));
    };
    
    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("Generate", "RoomEditor").Pressed())
        {
            LOG("Generating path!");
            Vector<ECS::VolumeCoordKey> prevPath = path; 
            ECS::VolumeCoord startCoord = GetVolume().GetCenter();
            ECS::VolumeCoord endCoord = GetRoom().Connection.Get();
            double time = Utility::Time::Get().Total();
            RoomGenPath pathGenerator = RoomGenPath(startCoord, endCoord, static_cast<int>(time * 100));
            while (!pathGenerator.Step()) {} // TODO: Step path generation (it'll look really cool)
            path = pathGenerator.GetPath();
            path.insert(path.begin(), startCoord.key);
            path.push_back(endCoord.key);
            addChange(prevPath);
        }
    }

    if (Input::Action::Get("LM").Pressed())
    {
        SelectClosest();
        if (selectedIndex >= 0 && Input::Action::Get("Ctrl").Down()) // Maybe make a copy of selected!
        {
            path.insert(path.begin() + selectedIndex + 1, path[selectedIndex]);
            selectedIndex++;
        }
    }
    if (Input::Action::Get("LM").Down())
        MoveSelected();
    if (Input::Action::Get("LM").Released())
        if (selectedIndex >= 0)
            if (selectPath != path)
                addChange(selectPath);

    if (Input::Action::Get("Remove", "RoomEditor").Pressed())
    {
        path.erase(path.begin() + selectedIndex);
        selectedIndex = -1;
        addChange(selectPath);
    }
    
    if (Input::Action::Get("RM").Pressed() && Input::Action::Get("Ctrl").Down())
    {
        SelectClosest();
        if (selectedIndex >= 0)
        {
            path.erase(path.begin() + selectedIndex);
            selectedIndex = -1;
            addChange(selectPath);
        }
    }
}

void RoomPathEditor::SmoothPath()
{
    auto& volume = GetVolume();
    float lerpSpeed = config.LerpSpeed.Get();
    float minLerpDist = config.MinLerpDist.Get();
    auto& path = GetRoom().Path.Get();
    if (smoothPath.size() != path.size())
    {
        smoothPath.resize(path.size());
        renderCacheChanged = true;
    }
    
    for (int i = 0; i < static_cast<int>(path.size()); ++i)
    {
        Vec3F targetPos = volume.CoordToPos(path[i]);
        Vec3F currPos = smoothPath[i];
        if (currPos == Vec3F::Zero())
        {
            smoothPath[i] = targetPos;
            renderCacheChanged = true;
            continue;
        }
        
        float lenSqr = (currPos - targetPos).LengthSqr(); 
        if (lenSqr > minLerpDist * minLerpDist)
        {
            Vec3F newPos = Utility::Math::Lerp(currPos, targetPos, lerpSpeed);
            smoothPath[i] = newPos;
            renderCacheChanged = true;
        }
    }
}

void RoomPathEditor::Enter()
{
}

void RoomPathEditor::Exit()
{
    // Only show when active
    auto& meshes = GetEditor().GetRenderScene().Meshes();
    meshes.Remove(pointHash, pointID);
    meshes.Remove(linkHash, linkID);
    smoothPath.clear();
}

void RoomPathEditor::Frame()
{
    CHECK_RETURN(!IsCurrent());
    CHECK_RETURN(!renderCacheChanged);
        
    auto& meshes = GetEditor().GetRenderScene().Meshes();
    meshes.Remove(pointHash, pointID);
    meshes.Remove(linkHash, linkID);
    
    Vec3F prevPos = Vec3F::Zero();
    Vector<Mat4F> links;
    Vector<Mat4F> points;
    for (int i = 0; i < static_cast<int>(smoothPath.size()); ++i)
    {
        Vec3F pos = smoothPath[i] + Vec3F::Up();
        float scale = i == selectedIndex ? config.SelectedScale : config.Scale;
        points.emplace_back(pos, QuatF::Identity(), scale);
        if (prevPos != Vec3F::Zero())
        {
            const Vec3F diff = prevPos - pos;
            QuatF rot = QuatF::FromDirection(diff);
            const Vec3F middlePos = (prevPos + pos) / 2;
            float s = config.Scale * 0.5f;
            const Vec3F size = Vec3F(s, diff.Length() * 0.5f, s);
            links.emplace_back(middlePos, rot, size);
        }
        prevPos = pos; 
    }

    MeshInstance link {
        .model = config.PathLink,
        .material = config.PathMaterial,
        .hash = linkHash
    };
    MeshInstance point {
        .model = config.PathPoint,
        .material = config.PathMaterial,
        .hash = pointHash
    };
    meshes.Add(link, links, pointID);
    meshes.Add(point, points, linkID);
}

void RoomPathEditor::VerifyPath() const
{
    auto& room = GetRoom();
    auto& path = room.Path.Get();

    auto startCoord = GetVolume().GetCenter().key;
    auto endCoord = room.Connection.Get();

    if (path.empty() || path.front() != startCoord)
        path = { startCoord };

    if (path.back() != endCoord)
    {
        for (int i = static_cast<int>(path.size()) - 1; i >= 0; i--)
            if (ECS::VolumeCoord(path[i]).pos.z >= ECS::VolumeCoord(endCoord).pos.z)
                path.erase(path.begin() + i);
        path.push_back(endCoord);
    }
}

void RoomPathEditor::SelectClosest()
{
    auto& cam = GetEditor().GetEditorCamera();
    Vec3F camPos = cam.GetPosition();
    Vec3F mouseDir = cam.GetMouseDirection();

    auto& room = GetRoom();
    auto& path = room.Path.Get();
    auto& v = GetVolume();
    int min = -1;
    float minDot = -1;
    for (int i = 1; i < static_cast<int>(path.size()); i++) // Can't select first
    {
        Vec3F pos = v.CoordToPos(path[i]);
        Vec3F dir = (pos - camPos).GetNormalized();
        float dot = Vec3F::Dot(mouseDir, dir);
        if (dot > minDot && dot > config.DotThreshold)
        {
            min = i;
            minDot = dot;
        }
    }

    selectedIndex = min;
    if (selectedIndex > 0)
    {
        selectPos = v.CoordToPos(path.at(selectedIndex)); 
        selectPath = path; // For history 
    }
}

void RoomPathEditor::MoveSelected()
{
    CHECK_RETURN(selectedIndex < 0);
    auto& coord = GetRoom().Path.Get().at(selectedIndex);
    auto& v = GetVolume();
    Vec3F newPos = DragMove(v.CoordToPos(coord));
    coord = v.PosToCoord(newPos).key;
}
