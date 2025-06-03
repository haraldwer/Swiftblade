#pragma once
#include "DBData.h"

namespace DB
{
	// - Common types - //

	enum class LBType : uint8
	{
		PERSONAL,
		FRIENDS,
		GLOBAL
	};

	enum class LBSeed : uint8
	{
		ANY,
		LAST_PLAYED,
		SPECIFIC
	};

	// - Recieved data - // 

	struct LBEntry : DBData<LBEntry>
	{
		PROPERTY_D(String, Name, "");
		PROPERTY_D(String, User, "");
		PROPERTY_D(int32, Rank, -1);
		PROPERTY_D(int32, Score, -1);
		PROPERTY_D(int32, Seed, -1);
	};

	struct LBData : DBData<LBData>
	{
		PROPERTY_D(uint8, Type, static_cast<uint8>(LBType::PERSONAL));
		PROPERTY_D(uint8, SeedType, static_cast<uint8>(LBSeed::ANY));
		PROPERTY_D(int32, Seed, -1);
		PROPERTY(Vector<LBEntry>, Entries); 
	};

	// - Submissions - // 

	struct LBSubmission : DBData<LBData>
	{
		PROPERTY_D(int32, Score, 0);
		PROPERTY_D(int32, Seed, -1);
	};

	// - Requests - //

	struct LBRequest : DBData<LBData>
	{
		PROPERTY_D(uint8, Type, static_cast<uint8>(LBType::PERSONAL));
		PROPERTY_D(uint8, SeedType, static_cast<uint8>(LBSeed::ANY));
		PROPERTY_D(int32, Seed, -1);
		PROPERTY_D(int32, Entries, 10);

		bool operator==(const LBRequest& InOther) const
		{
			return Type == InOther.Type &&
				SeedType == InOther.SeedType &&
				Seed == InOther.Seed &&
				Entries == InOther.Entries;
		}
	};

	// - Broadcasts - //
	
	struct OnListSuccess
	{
		LBData data;
	};

	struct OnWriteSuccess
	{
		String result;
	};
	
	struct OnListError : OnError {};
	struct OnWriteError : OnError {};

	// - Blob - //

	struct BlobLB : DBData<BlobLB>
	{
		PROPERTY(Vector<LBEntry>, Entries);
	};
}
