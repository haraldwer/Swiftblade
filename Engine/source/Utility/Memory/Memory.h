
#pragma once

#include <cstdint>
#include <cstdlib>

namespace Utility
{
	namespace Memory
	{
		template<typename To, typename From>
		To& Cast(From *InAddr, int InByteOff = 0)
		{
			return *reinterpret_cast<To *>(reinterpret_cast<char *>(InAddr) + InByteOff);
		}
		
		class Data
		{
		public:
			int64_t sizeOfData = 0;
			void* buffer = nullptr;

			void Delete()
			{
				free(buffer);
				buffer = nullptr;
				sizeOfData = 0;
			}

			Data(const int64_t InNewSizeOfData, void* InNewBuffer) :
				sizeOfData(InNewSizeOfData),
				buffer(InNewBuffer) {}
			~Data() { Delete(); }
		};
	}
}
