
#pragma once

#include <cstdint>
#include <cstdlib>

namespace Utility
{
	namespace Memory
	{
		template<typename To, typename From>
		To& Cast(From *aAddress, int aByteOffset = 0)
		{
			return *reinterpret_cast<To *>(reinterpret_cast<char *>(aAddress) + aByteOffset);
		}
		
		class Data
		{
		public:
			int64_t size_of_data = 0;
			void* buffer = nullptr;

			void Delete()
			{
				free(buffer);
				buffer = nullptr;
				size_of_data = 0;
			}

			Data(int64_t new_size_of_data, void* new_buffer) :
				size_of_data(new_size_of_data),
				buffer(new_buffer) {}
			~Data() { Delete(); }
		};
	}
}
