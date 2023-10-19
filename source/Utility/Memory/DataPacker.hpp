#pragma once

#include <vector>
#include <cstdint>

namespace Utility
{
	namespace Memory
	{
		class DataPacker
		{
			
		public:

			char* GetDataBuffer()
			{
				return data.data();
			}

			int GetDataBufferSize() const
			{
				return used_bytes;
			}
			
			template<typename PackedType>
			DataPacker& operator << (PackedType aValue)
			{
				const uint32_t type_size = sizeof(PackedType);
				RequestSize(type_size);
				memcpy(&data[position], &aValue, type_size);
				position += type_size;
				used_bytes += type_size;
				return *this;
			}

			template<typename Type>
			DataPacker& operator << (std::vector<Type> aValue)
			{
				const uint32_t number_of_elements = aValue.size();

				(*this) << number_of_elements;
				const int data_size = number_of_elements * sizeof(Type);
				RequestSize(number_of_elements*sizeof(Type));

				memcpy(&data[position], aValue.data(), data_size);

				position += data_size;
				used_bytes += data_size;
				return *this;
			}

			template<typename UnpackedType>
			DataPacker& operator >> (UnpackedType& aValue)
			{
				aValue = (*(UnpackedType*)(&data[position]));
				position += sizeof(aValue);
				return *this;
			}

			template<typename Type>
			DataPacker& operator >> (std::vector<Type>& aValue)
			{
				int32_t number_of_elements = 0;
				(*this) >> number_of_elements;
				aValue.resize(number_of_elements);
				const int data_size = number_of_elements * sizeof(Type);

				memcpy(aValue.data(), (&data[position]), data_size);

				position += data_size;
				return *this;
			}

			void SetPosition(int aPosition)
			{
				position = aPosition;
			}

			void Skip(int aBytes)
			{
				position += aBytes;
			}

			void RequestSize(int aBytes)
			{
				if (data.size() - used_bytes <= aBytes)
				{
					Expand(aBytes);
				}
			}

			void Clear()
			{
				used_bytes = 0;
				position = 0;
			}

			DataPacker() : used_bytes(0) {}
			~DataPacker() = default;

		private:

			int used_bytes;
			std::vector<char> data;
			int position = 0;
			
			void Expand(int aMinimum)
			{
				const int expansion_size = aMinimum > 256 ? aMinimum : 256;
				data.resize(expansion_size+data.size());
			}
		};

		class DataUnpacker
		{
		public:

			template<typename UnpackedType>
			DataUnpacker& operator >> (UnpackedType& aValue)
			{
				aValue = (*(UnpackedType*)(&data[position]));
				position += sizeof(aValue);
				return *this;
			}

			template<typename UnpackedType>
			operator UnpackedType()
			{
				position += sizeof(UnpackedType);

				return *(UnpackedType*)(&data[position - sizeof(UnpackedType)]);
			}
			

			DataUnpacker(void* data) : data((char*)data) {}
			~DataUnpacker() = default;

		private:
			
			char* data = nullptr;
			int position = 0;
		};
	}
}