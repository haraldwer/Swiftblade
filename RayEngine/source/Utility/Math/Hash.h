#pragma once

namespace Utility
{
	inline uint32 Hash(const String& aKey)
	{
		const auto* aBuffer = reinterpret_cast<const uint8*>(aKey.c_str());
		const size_t count = aKey.size();

		constexpr uint32 FNVOffsetBasis = 2166136261U;
		constexpr uint32 FNVPrime = 16777619U;
		uint32 val = FNVOffsetBasis;
		for (size_t i = 0; i < count; i++)
		{
			val ^= aBuffer[i];
			val *= FNVPrime;
		}
		return val;
	}
	
	template <class T>
	uint32 Hash(T aKey)
	{
		const uint8* aBuffer = reinterpret_cast<const uint8*>(&aKey);
		const int count = sizeof(T); 
		
		constexpr uint32 FNVOffsetBasis = 2166136261U;
		constexpr uint32 FNVPrime = 16777619U;
		uint32 val = FNVOffsetBasis;
		for(int i = 0; i < count; i++)
		{
			val ^= aBuffer[i];
			val *= FNVPrime;
		}
		return val;
	}
}
	