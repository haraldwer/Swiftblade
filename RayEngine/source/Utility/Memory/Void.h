#pragma once

namespace Utility
{
    namespace Memory
    {
        // A data type that should eliminate all direct or indirect interactions made with it.
        struct Void
        {
        public:
            template<typename DataType>
            operator DataType() const { return DataType(); }

            template<typename DataType>
            Void operator =(const DataType) const { return *this; }

            template<typename DataType>
            Void operator +(const DataType) const { return *this; }

            template<typename DataType>
            Void operator -(const DataType) const { return *this; }

            template<typename DataType>
            Void operator *(const DataType) const { return *this; }

            template<typename DataType>
            Void operator /(const DataType) const { return *this; }

            template<typename DataType>
            Void operator %(const DataType) const { return *this; }

            template<typename DataType>
            Void operator +=(const DataType) const { return *this; }

            template<typename DataType>
            Void operator -=(const DataType) const { return *this; }

            template<typename DataType>
            Void operator *=(const DataType) const { return *this; }

            template<typename DataType>
            Void operator /=(const DataType) const { return *this; }

            Void() = default;
        };
    }
}
