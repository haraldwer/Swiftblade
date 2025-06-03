#pragma once

#include <vector>
#include <cstdint>


namespace Utility::Memory
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
			return usedBytes;
		}
		
		template<typename PackedType>
		DataPacker& operator << (PackedType InValue)
		{
			const uint32_t type_size = sizeof(PackedType);
			RequestSize(type_size);
			memcpy(&data[position], &InValue, type_size);
			position += type_size;
			usedBytes += type_size;
			return *this;
		}

		template<typename Type>
		DataPacker& operator << (std::vector<Type> InValue)
		{
			const uint32_t number_of_elements = InValue.size();

			(*this) << number_of_elements;
			const int data_size = number_of_elements * sizeof(Type);
			RequestSize(number_of_elements*sizeof(Type));

			memcpy(&data[position], InValue.data(), data_size);

			position += data_size;
			usedBytes += data_size;
			return *this;
		}

		template<typename UnpackedType>
		DataPacker& operator >> (UnpackedType& InValue)
		{
			InValue = (*(UnpackedType*)(&data[position]));
			position += sizeof(InValue);
			return *this;
		}

		template<typename Type>
		DataPacker& operator >> (std::vector<Type>& InValue)
		{
			int32_t number_of_elements = 0;
			(*this) >> number_of_elements;
			InValue.resize(number_of_elements);
			const int data_size = number_of_elements * sizeof(Type);

			memcpy(InValue.data(), (&data[position]), data_size);

			position += data_size;
			return *this;
		}

		void SetPosition(int InPosition)
		{
			position = InPosition;
		}

		void Skip(const int InBytes)
		{
			position += InBytes;
		}

		void RequestSize(int InBytes)
		{
			if (data.size() - usedBytes <= InBytes)
			{
				Expand(InBytes);
			}
		}

		void Clear()
		{
			usedBytes = 0;
			position = 0;
		}

		DataPacker() : usedBytes(0) {}
		~DataPacker() = default;

	private:

		int usedBytes;
		std::vector<char> data;
		int position = 0;
		
		void Expand(int InMinimum)
		{
			const int expansionSize = InMinimum > 256 ? InMinimum : 256;
			data.resize(expansionSize+data.size());
		}
	};

	class DataUnpacker
	{
	public:

		template<typename UnpackedType>
		DataUnpacker& operator >> (UnpackedType& InValue)
		{
			InValue = (*(UnpackedType*)(&data[position]));
			position += sizeof(InValue);
			return *this;
		}

		template<typename UnpackedType>
		operator UnpackedType()
		{
			position += sizeof(UnpackedType);

			return *(UnpackedType*)(&data[position - sizeof(UnpackedType)]);
		}
		

		DataUnpacker(void* InData) : data((char*)InData) {}
		~DataUnpacker() = default;

	private:
		
		char* data = nullptr;
		int position = 0;
	};
}
