#include "RoomResource.h"

bool RoomResource::Load(const String &InIdentifier)
{
    identifier = InIdentifier;
    return room.Load(InIdentifier);
}

bool RoomResource::Unload()
{
    return room.Unload();
}

Utility::Timepoint RoomResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(identifier);
}

bool RoomResource::Save(const String &InPath) const
{
    return room.Save(InPath);
}

bool RoomResource::Edit(const String &InName, uint32 InOffset)
{
    return room.Edit(InName, InOffset);
}
