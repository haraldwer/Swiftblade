#include "RoomPathEditor.h"

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

    if (persistentID == 0)
    {
        persistentID = MeshInstance::GenPersistentID();
        pointHash = MeshInstance::GenHash(config.PathPoint.Get(), config.PathMaterial.Get());
        linkHash = MeshInstance::GenHash(config.PathLink.Get(), config.PathMaterial.Get());
    }
}

void RoomPathEditor::Deinit()
{
    auto& meshes = GetEditor().GetRenderScene().Meshes();
    meshes.Remove(pointHash, persistentID);
    meshes.Remove(linkHash, persistentID);
    config.SaveConfig();
}

void RoomPathEditor::Update()
{
    if (!IsCurrent())
        return;
    
    VerifyPath();

    auto& path = GetRoom().Path.Get();
    
    // Select
    // Move
    // Remove 
    // Add
    // Generate
    // Clear all

    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("Generate").Pressed())
        {
            auto startCoord = GetVolume().GetCenter().key;
            auto endCoord = GetRoom().Connection.Get();
            RoomGenPath pathGenerator = RoomGenPath(startCoord, endCoord, 0);
            while (pathGenerator.Step()) {} // TODO: Step path generation (it'll look really cool)
            path = pathGenerator.GetPath();
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
    meshes.Remove(pointHash, persistentID);
    meshes.Remove(linkHash, persistentID);
    
    Vec3F prevPos = Vec3F::Zero();
    Vector<Mat4F> links;
    Vector<Mat4F> points;
    float scale = config.Scale.Get();
    for (auto c : GetRoom().Path.Get())
    {
        const ECS::VolumeCoord coord(c);
        Vec3F pos = volume.CoordToPos(coord);
        points.push_back(Mat4F(pos, QuatF::Identity(), scale));
        if (prevPos != Vec3F::Zero())
        {
            const Vec3F diff = prevPos - pos;
            const float dist = diff.Length();
            const QuatF rot = QuatF::FromDirection(diff / dist);
            const Vec3F middlePos = (prevPos + pos) / 2;
            links.push_back(Mat4F(middlePos, rot, Vec3F(scale, scale, dist)));
        }
        prevPos = pos; 
    }

    MeshInstance link {
        .model = config.PathLink.Get(),
        .material = config.PathMaterial.Get(),
        .hash = linkHash
    };
    MeshInstance point {
        .model = config.PathPoint.Get(),
        .material = config.PathMaterial.Get(),
        .hash = linkHash
    };
    meshes.Add(link, links, persistentID);
    meshes.Add(point, points, persistentID);
}

void RoomPathEditor::VerifyPath()
{
    auto& room = GetRoom();
    auto& path = room.Path.Get();

    auto startCoord = GetVolume().GetCenter().key;
    auto endCoord = room.Connection.Get();

    if (path.size() < 2 || path.front() != startCoord || path.back() != endCoord)
    {
        path = { startCoord, endCoord };
        renderCacheChanged = true;
    }
}
