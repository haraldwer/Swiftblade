#pragma once

#include <cassert>

template<typename Type>
Type DefaultInitializationValue()
{
    assert(false && "DataType has no supported default initialization value.");
    return {};
}

template<>
inline float DefaultInitializationValue<float>()
{
    return 0.0f;
}

template<>
inline double DefaultInitializationValue<double>()
{
    return 0.0;
}

template<>
inline int DefaultInitializationValue<int>()
{
    return 0;
}

template<>
inline unsigned int DefaultInitializationValue<unsigned int>()
{
    return 0;
}

template<>
inline unsigned char DefaultInitializationValue<unsigned char>()
{
    return 0;
}

template<>
inline char DefaultInitializationValue<char>()
{
    return 0;
}

template<>
inline int8 DefaultInitializationValue<int8>()
{
    return 0;
}

