#pragma once
#include "DBData.h"

namespace DB
{
	struct ToombstoneEntry : DBData<ToombstoneEntry>
	{
		PROPERTY_D(int32, Seed, -1);
		PROPERTY_D(int32, ArenaIndex, -1);
		PROPERTY(Vec3F, Location);
	};

	struct BlobToombstone : DBData<BlobToombstone>
	{
		PROPERTY(Vector<ToombstoneEntry>, Toombstones); 
	};
}
