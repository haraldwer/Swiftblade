#include "RoomGenVolume.h"

#include "ECS/Volume/CubeVolume.h"
#include "Editor/SubEditors/RoomGenEditor.h"

void RoomGenVolume::Clear()
{
    QueuedCoords.clear();
    NextQueue.clear();
    CheckedCoords.clear();
    Result.clear();
    VolumeDepth = 0;
}

void RoomGenVolume::Init()
{
    CHECK_RETURN(!Owner);
    for (const Coord& c : Owner->PathGen.GetPath())
        QueuedCoords.push_back({c.Key, c.Key}); 
}

void RoomGenVolume::TryQueueEntry(const Coord InNewCoord, const Coord InReference)
{
    if (CheckedCoords[InReference.Key].contains(InNewCoord.Key))
        return;
    CheckedCoords[InReference.Key].insert(InNewCoord.Key); 
    NextQueue[InNewCoord.Key] = InReference.Key;
}

bool RoomGenVolume::EvaluateCoord(const Coord InCoord, const Coord InReference, uint8& InOutValue)
{
    CHECK_RETURN(!Owner, false);
    
    // Start and end
    Coord front = Owner->PathGen.GetPath().front();
    Coord back = Owner->PathGen.GetPath().back();
    if (InCoord.Pos.Z <= front.Pos.Z ||
        InCoord.Pos.Z >= back.Pos.Z)
    {
        if (InCoord.Pos.Z == front.Pos.Z || InCoord.Pos.Z == back.Pos.Z)
        {
            Coord comp = InCoord.Pos.Z == front.Pos.Z ? front : back;
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
    if (Result.contains(InCoord.Key))
        InOutValue = Result[InCoord.Key];
        
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
    CHECK_RETURN(!Owner, true);
    
    auto& v = Owner->GetVolume();

    for (int i = 0; i < 200; i++)
    {
        if (QueuedCoords.empty())
            break;
        QueuedEntry entry = QueuedCoords.front();
        QueuedCoords.erase(QueuedCoords.begin());
        
        uint8 val = 0;
        bool success = EvaluateCoord(entry.Coord, entry.Ref, val);

        uint8& existingValue = Result[entry.Coord];
        if (existingValue != val)
        {
            existingValue = val;
            if (val > 0)
                v.Data[entry.Coord] = val;
            else if (v.Data.contains(entry.Coord))
                v.Data.erase(entry.Coord);
        }

        if (!success)
            continue;

        for (Coord c : ECS::CubeVolume::GetNeighbors(entry.Coord))
            if (c.Key != 0)
                TryQueueEntry(c, entry.Ref);
    }

    if (QueuedCoords.empty())
    {
        for (auto& q : NextQueue)
            QueuedCoords.push_back({q.first, q.second});
        NextQueue.clear();
        
        VolumeDepth++;
        v.UpdateCache(Mat4F());
    }

    if (QueuedCoords.empty())
    {
        v.Data.clear();
        for (auto& val : Result)
            if (val.second > 0)
                v.Data[val.first] = val.second;
        v.UpdateCache(Mat4F());
        return true;
    }
    
    return false;
}
