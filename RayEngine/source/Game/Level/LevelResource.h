#pragma once
#include "Level.h"
#include "Resource/PropertyFile.h"
#include "Resource/Resource.h"

typedef Resource::PropertyFile<Level> LevelResource;
typedef Resource::Ref<LevelResource> ResLevel;