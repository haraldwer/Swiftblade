#pragma once

#include <cstdint>

#include "Void.h"

namespace Utility
{
	namespace Memory
	{
		// Wrapper for treating variables as if they're in a different position in memory.
		// Warning, use with extreme care!
		template<typename DataType, int64_t ByteOffset = 0>
		struct Map
		{
		public:

			DataType& operator ()() const
			{
				return (*reinterpret_cast<DataType*>(reinterpret_cast<char*>(const_cast<Map*>(this)) + ByteOffset));
			}

			operator DataType& () const
			{
				return (*reinterpret_cast<DataType*>(reinterpret_cast<char*>(const_cast<Map*>(this)) + ByteOffset));
			}

			operator Map& () = delete; // todo: test what the consequences of this actually are.

			//template<typename AssignedDataType>
			DataType& operator = (const DataType &InNewValue)
			{
				return (*this)() = InNewValue;
			}

			DataType& operator = (const Map &InNewValue)
			{
				return (*this)() = InNewValue();
			}

			Void &operator = (Memory::Void& InNewValue) const
			{
				return InNewValue;
			}
			

			DataType operator + (DataType &InMemoryB)
			{
				return ((*this)() + InMemoryB);
			}

			DataType operator - (DataType InMemoryB)
			{
				return ((*this)() - InMemoryB);
			}

			DataType operator * (DataType InMemoryB)
			{
				return ((*this)() * InMemoryB);
			}

			DataType operator / (DataType InMemoryB)
			{
				return ((*this)() / InMemoryB);
			}

			DataType operator % (DataType InMemoryB)
			{
				return ((*this)() % InMemoryB);
			}


			DataType& operator += (DataType InMemoryB)
			{
				return ((*this)() += InMemoryB);
			}

			DataType& operator -= (DataType InMemoryB)
			{
				return ((*this)() -= InMemoryB);
			}

			DataType& operator *= (DataType InMemoryB)
			{
				return ((*this)() *= InMemoryB);
			}

			DataType& operator /= (DataType InMemoryB)
			{
				return ((*this)() /= InMemoryB);
			}

			DataType& operator %= (DataType InMemoryB)
			{
				return ((*this)() %= InMemoryB);
			}


			Map(DataType InInitialValue) { (*this) = InInitialValue; }
		};
	}
}
