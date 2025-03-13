#pragma once

#include <cstdint>

#include "Void.h"

namespace Utility
{
	namespace Memory
	{
		// Wrapper for treating variables as if they're in a different position in memory.
		// Warning, use with extreme care!
		template<typename DataType, int64_t byte_offset = 0>
		struct Map
		{
		public:

			DataType& operator ()() const
			{
				return (*reinterpret_cast<DataType*>(reinterpret_cast<char*>(const_cast<Map*>(this)) + byte_offset));
			}

			operator DataType& () const
			{
				return (*reinterpret_cast<DataType*>(reinterpret_cast<char*>(const_cast<Map*>(this)) + byte_offset));
			}

			operator Map& () = delete; // todo: test what the consequences of this actually are.

			//template<typename AssignedDataType>
			DataType& operator = (const DataType &new_value)
			{
				return (*this)() = new_value;
			}

			DataType& operator = (const Map &new_value)
			{
				return (*this)() = new_value();
			}

			Memory::Void &operator = (Memory::Void& new_value) const
			{
				return new_value;
			}
			

			DataType operator + (DataType &memory_b)
			{
				return ((*this)() + memory_b);
			}

			DataType operator - (DataType memory_b)
			{
				return ((*this)() - memory_b);
			}

			DataType operator * (DataType memory_b)
			{
				return ((*this)() * memory_b);
			}

			DataType operator / (DataType memory_b)
			{
				return ((*this)() / memory_b);
			}

			DataType operator % (DataType memory_b)
			{
				return ((*this)() % memory_b);
			}


			DataType& operator += (DataType memory_b)
			{
				return ((*this)() += memory_b);
			}

			DataType& operator -= (DataType memory_b)
			{
				return ((*this)() -= memory_b);
			}

			DataType& operator *= (DataType memory_b)
			{
				return ((*this)() *= memory_b);
			}

			DataType& operator /= (DataType memory_b)
			{
				return ((*this)() /= memory_b);
			}

			DataType& operator %= (DataType memory_b)
			{
				return ((*this)() %= memory_b);
			}


			Map(DataType initial_value) { (*this) = initial_value; }
		};
	}
}
