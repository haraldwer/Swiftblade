#pragma once
#include "Room.h"
#include "Resource/PropertyFile.h"
#include "Resource/Resource.h"

typedef Resource::PropertyFile<Room> RoomResource;
typedef Resource::Ref<RoomResource> ResRoom;

// TODO: Make net resource