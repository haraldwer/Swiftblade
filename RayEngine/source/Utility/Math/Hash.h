#pragma once

namespace Utility
{
	inline uint32 Hash(const String& InKey)
	{
		const auto* aBuffer = reinterpret_cast<const uint8*>(InKey.c_str());
		const size_t count = InKey.size();

		constexpr uint32 fnvOffsetBasis = 2166136261U;
		constexpr uint32 fnvPrime = 16777619U;
		uint32 val = fnvOffsetBasis;
		for (size_t i = 0; i < count; i++)
		{
			val ^= aBuffer[i];
			val *= fnvPrime;
		}
		return val;
	}
	
	template <class T>
	uint32 Hash(T InKey)
	{
		const uint8* aBuffer = reinterpret_cast<const uint8*>(&InKey);
		const int count = sizeof(T); 
		
		constexpr uint32 fnvOffsetBasis = 2166136261U;
		constexpr uint32 fnvPrime = 16777619U;
		uint32 val = fnvOffsetBasis;
		for(int i = 0; i < count; i++)
		{
			val ^= aBuffer[i];
			val *= fnvPrime;
		}
		return val;
	}
}
	