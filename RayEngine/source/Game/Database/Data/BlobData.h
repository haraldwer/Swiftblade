#pragma once

#include "DBData.h"
#include "LBData.h"
#include "ToombstoneData.h"

namespace DB
{
	struct BlobSeed : DBData<BlobSeed>
	{
		PROPERTY_D(int32, Seed, 0);
	};

	struct BlobData : DBData<BlobData>
	{
		PROPERTY_D(bool, Initialized, false); 
		PROPERTY(BlobSeed, SeedData);
		//PROPERTY(BlobLB, LBData);
		//PROPERTY(BlobToombstone, myToombstoneData);
	};
}

