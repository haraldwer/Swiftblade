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
    renderCacheChanged = true;

    if (pointID == 0)
    {
        pointID = MeshInstance::GenPersistentID();
        linkID = MeshInstance::GenPersistentID();
        pointHash = MeshInstance::GenHash(config.PathPoint.Get(), config.PathMaterial.Get());
        linkHash = MeshInstance::GenHash(config.PathLink.Get(), config.PathMaterial.Get());
    }
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
    auto& path = GetRoom().Path.Get();
    
    // Select
    // Move
    // Remove 
    // Add
    // Generate
    // Clear all

    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("Generate", "RoomEditor").Pressed())
        {
            LOG("Generating path!");
            ECS::VolumeCoord startCoord = GetVolume().GetCenter();
            ECS::VolumeCoord endCoord = GetRoom().Connection.Get();
            double time = Utility::Time::Get().Total();
            RoomGenPath pathGenerator = RoomGenPath(startCoord, endCoord, static_cast<int>(time * 100));
            while (!pathGenerator.Step()) {} // TODO: Step path generation (it'll look really cool)
            path = pathGenerator.GetPath();
            path.insert(path.begin(), startCoord.key);
            path.push_back(endCoord.key);
            renderCacheChanged = true;
        }
    }
}

void RoomPathEditor::Frame()
{
    if (!renderCacheChanged)
        return;
    renderCacheChanged = false;
        
    auto& volume = GetVolume();
    auto& meshes = GetEditor().GetRenderScene().Meshes();
    meshes.Remove(pointHash, pointID);
    meshes.Remove(linkHash, linkID);
    
    Vec3F prevPos = Vec3F::Zero();
    Vector<Mat4F> links;
    Vector<Mat4F> points;
    float scale = config.Scale.Get();
    for (auto c : GetRoom().Path.Get())
    {
        const ECS::VolumeCoord coord(c);
        Vec3F pos = volume.CoordToPos(coord) + Vec3F::Up();
        points.emplace_back(pos, QuatF::Identity(), scale);
        if (prevPos != Vec3F::Zero())
        {
            const Vec3F diff = prevPos - pos;
            QuatF rot = QuatF::FromDirection(diff);
            //rot *= QuatF::FromEuler(Vec3F(PI/2, 0, 0));
            const Vec3F middlePos = (prevPos + pos) / 2;
            const Vec3F size = Vec3F(scale * 0.5f, diff.Length() * 0.5f, scale * 0.5f);
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

void RoomPathEditor::VerifyPath()
{
    auto& room = GetRoom();
    auto& path = room.Path.Get();

    auto startCoord = GetVolume().GetCenter().key;
    auto endCoord = room.Connection.Get();

    if (path.empty() || path.front() != startCoord)
    {
        path = { startCoord };
        renderCacheChanged = true;
    }

    if (path.back() != endCoord)
    {
        for (int i = static_cast<int>(path.size()) - 1; i >= 0; i--)
            if (ECS::VolumeCoord(path[i]).pos.z >= ECS::VolumeCoord(endCoord).pos.z)
                path.erase(path.begin() + i);
        path.push_back(endCoord);
        renderCacheChanged = true;
    }
}
