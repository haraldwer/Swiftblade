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
    for (const Coord& c : owner->pathGen.GetPath())
        queuedCoords.push_back({c.Key, c.Key}); 
}

void RoomGenVolume::TryQueueEntry(const Coord InNewCoord, const Coord InReference)
{
    if (checkedCoords[InReference.Key].contains(InNewCoord.Key))
        return;
    checkedCoords[InReference.Key].insert(InNewCoord.Key); 
    nextQueue[InNewCoord.Key] = InReference.Key;
}

bool RoomGenVolume::EvaluateCoord(const Coord InCoord, const Coord InReference, uint8& InOutValue)
{
    CHECK_RETURN(!owner, false);
    
    // Start and end
    const Coord front = owner->pathGen.GetPath().front();
    const Coord back = owner->pathGen.GetPath().back();
    if (InCoord.Pos.Z <= front.Pos.Z ||
        InCoord.Pos.Z >= back.Pos.Z)
    {
        if (InCoord.Pos.Z == front.Pos.Z || InCoord.Pos.Z == back.Pos.Z)
        {
            const Coord comp = InCoord.Pos.Z == front.Pos.Z ? front : back;
            if (InCoord.Pos.X - comp.Pos.X > 0 ||
                InCoord.Pos.X - comp.Pos.X < -1 ||
                InCoord.Pos.Y - comp.Pos.Y > 2 ||
                InCoord.Pos.Y - comp.Pos.Y < 0)
            {
                InOutValue = 1; // Wall
                return false;
            }
            return true; // Continue, to get ground below entrance
        }
        return false;
    }

    InOutValue = 1;
    if (result.contains(InCoord.Key))
        InOutValue = result[InCoord.Key];
        
    // Entrance and exit ground cap
    bool checkHeight = true;
    if (abs(InCoord.Pos.X - front.Pos.X) < 1 &&
        abs(InCoord.Pos.Z - front.Pos.Z) < 1)
        if (InCoord.Pos.Y >= front.Pos.Y)
            checkHeight = false;
    if (abs(InCoord.Pos.X - back.Pos.X) < 1 &&
        abs(InCoord.Pos.Z - back.Pos.Z) < 1)
        if (InCoord.Pos.Y >= back.Pos.Y)
            checkHeight = false;
        
    // Height
    if (checkHeight)
    {
        const int height = InCoord.Pos.Y - InReference.Pos.Y;
        if (height > 7 || height < 0)
            return false;
    }

    // Distance
    if (!(abs(InCoord.Pos.X - InReference.Pos.X) < 7 &&
        abs(InCoord.Pos.Z - InReference.Pos.Z) < 7))
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
                v.Data[entry.coord] = val;
            else if (v.Data.contains(entry.coord))
                v.Data.erase(entry.coord);
        }

        if (!success)
            continue;

        for (const Coord c : ECS::CubeVolume::GetNeighbors(entry.coord))
            if (c.Key != 0)
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
        v.Data.clear();
        for (auto& val : result)
            if (val.second > 0)
                v.Data[val.first] = val.second;
        v.UpdateCache(Mat4F());
        return true;
    }
    
    return false;
}
