#pragma once
#include "Room.h"
#include "Resource/Resource.h"

class RoomResource
{
public:

    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const;
    
    Room& Get() { return room; }

    bool Save(const String& InPath) const;
    bool Edit(const String& InName, uint32 InOffset = 0);

private:

    Room room;
    String identifier = {};
    
};

typedef Resource::Ref<RoomResource> ResRoom;