#pragma once

#include "DBData.h"

namespace DB
{
	struct BlobSeed : DBData<BlobSeed>
	{
		PROPERTY_D(int32, Seed, 0);
	};

	struct BlobData : DBData<BlobData>
	{
		bool initialized = false; 
		PROPERTY(BlobSeed, SeedData);
		//PROPERTY(BlobLB, LBData);
		//PROPERTY(BlobToombstone, myToombstoneData);
	};

	struct OnBlobReadSuccess { };
	struct OnBlobWriteSuccess { };
	struct OnBlobReadError : OnError {};
	struct OnBlobWriteError : OnError {};
}

