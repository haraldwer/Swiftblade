#include "RoomGenVolume.h"

#include "ECS/Volume/CubeVolume.h"
#include "Editor/SubEditors/RoomGenEditor.h"

void RoomGenVolume::Clear()
{
    queuedCoords.clear();
    nextQueue.clear();
    checkedCoords.clear();
    result.clear();
    volumeDepth = 0;
}

void RoomGenVolume::Init()
{
    CHECK_RETURN(!owner);
    for (const ECS::VolumeCoord& c : owner->pathGen.GetPath())
        queuedCoords.push_back({c.key, c.key}); 
}

void RoomGenVolume::TryQueueEntry(const ECS::VolumeCoord InNewCoord, const ECS::VolumeCoord InReference)
{
    if (checkedCoords[InReference.key].contains(InNewCoord.key))
        return;
    checkedCoords[InReference.key].insert(InNewCoord.key); 
    nextQueue[InNewCoord.key] = InReference.key;
}

bool RoomGenVolume::EvaluateCoord(const ECS::VolumeCoord InCoord, const ECS::VolumeCoord InReference, uint8& InOutValue)
{
    CHECK_RETURN(!owner, false);
    
    // Start and end
    const ECS::VolumeCoord front = owner->pathGen.GetPath().front();
    const ECS::VolumeCoord back = owner->pathGen.GetPath().back();
    if (InCoord.pos.z <= front.pos.z ||
        InCoord.pos.z >= back.pos.z)
    {
        if (InCoord.pos.z == front.pos.z || InCoord.pos.z == back.pos.z)
        {
            const ECS::VolumeCoord comp = InCoord.pos.z == front.pos.z ? front : back;
            if (InCoord.pos.x - comp.pos.x > 0 ||
                InCoord.pos.x - comp.pos.x < -1 ||
                InCoord.pos.y - comp.pos.y > 2 ||
                InCoord.pos.y - comp.pos.y < 0)
            {
                InOutValue = 1; // Wall
                return false;
            }
            return true; // Continue, to get ground below entrance
        }
        return false;
    }

    InOutValue = 1;
    if (result.contains(InCoord.key))
        InOutValue = result[InCoord.key];
        
    // Entrance and exit ground cap
    bool checkHeight = true;
    if (abs(InCoord.pos.x - front.pos.x) < 1 &&
        abs(InCoord.pos.z - front.pos.z) < 1)
        if (InCoord.pos.y >= front.pos.y)
            checkHeight = false;
    if (abs(InCoord.pos.x - back.pos.x) < 1 &&
        abs(InCoord.pos.z - back.pos.z) < 1)
        if (InCoord.pos.y >= back.pos.y)
            checkHeight = false;
        
    // Height
    if (checkHeight)
    {
        const int height = InCoord.pos.y - InReference.pos.y;
        if (height > 7 || height < 0)
            return false;
    }

    // Distance
    if (!(abs(InCoord.pos.x - InReference.pos.x) < 7 &&
        abs(InCoord.pos.z - InReference.pos.z) < 7))
        return false;

    InOutValue = 0;
    return true;
}

bool RoomGenVolume::Step()
{
    CHECK_RETURN(!owner, true);
    
    auto& v = owner->GetVolume();

    for (int i = 0; i < 200; i++)
    {
        if (queuedCoords.empty())
            break;
        QueuedEntry entry = queuedCoords.front();
        queuedCoords.erase(queuedCoords.begin());
        
        uint8 val = 0;
        const bool success = EvaluateCoord(entry.coord, entry.ref, val);

        uint8& existingValue = result[entry.coord];
        if (existingValue != val)
        {
            existingValue = val;
            if (val > 0)
                v.data.data[entry.coord] = val;
            else if (v.data.data.contains(entry.coord))
                v.data.data.erase(entry.coord);
        }

        if (!success)
            continue;

        for (const ECS::VolumeCoord c : ECS::CubeVolume::GetNeighbors(entry.coord))
            if (c.key != 0)
                TryQueueEntry(c, entry.ref);
    }

    if (queuedCoords.empty())
    {
        for (const auto& q : nextQueue)
            queuedCoords.push_back({q.first, q.second});
        nextQueue.clear();
        
        volumeDepth++;
        v.UpdateCache(Mat4F());
    }

    if (queuedCoords.empty())
    {
        v.data.data.clear();
        for (auto& val : result)
            if (val.second > 0)
                v.data.data[val.first] = val.second;
        v.UpdateCache(Mat4F());
        return true;
    }
    
    return false;
}
