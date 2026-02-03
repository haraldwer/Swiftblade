#pragma once

// Slim windows
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS    
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES    
#define NOWINSTYLES      
#define NOSYSMETRICS     
#define NOMENUS          
#define NOICONS          
#define NOKEYSTATES      
#define NOSYSCOMMANDS    
#define NORASTEROPS      
#define NOSHOWWINDOW     
#define OEMRESOURCE      
#define NOATOM           
#define NOCLIPBOARD      
#define NOCOLOR          
#define NOCTLMGR         
#define NODRAWTEXT       
#define NOGDI            
#define NOKERNEL         
#define NOUSER           
#define NONLS            
#define NOMB             
#define NOMEMMGR         
#define NOMETAFILE       
#define NOMINMAX         
#define NOMSG            
#define NOOPENFILE       
#define NOSCROLL         
#define NOSERVICE        
#define NOSOUND          
#define NOTEXTMETRIC     
#define NOWH             
#define NOWINOFFSETS     
#define NOCOMM           
#define NOKANJI          
#define NOHELP           
#define NOPROFILER       
#define NODEFERWINDOWPOS 
#define NOMCX            

// Silence warnings
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include <set>
#include <array>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <chrono>
#include <utility>
#include <cassert>
#include <cstdint>
#include <functional>
#include <algorithm>

using String = std::string; 
template <class T>
using Set = std::set<T>;
template <class T>
using Vector = std::vector<T>;
template <class T, int Size>
using Array = std::array<T, Size>;
template <class Key, class Val>
using Map = std::unordered_map<Key, Val>;
template <class Val>
using StringMap = Map<String, Val>;  
template <class Key, class Val>
using OrderedMap = std::map<Key, Val>;

typedef uint64_t uint64;
typedef int64_t int64;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint8_t uint8;
typedef int8_t int8;

#include "Utility/Math/Macros.h"
#include "Utility/Math/Basic.h"
#include "Utility/Math/Lerp.h"

#include "Utility/Utility.h"

#include "Utility/Type.h"
using Type = Utility::Type;

#include "Utility/Memory/Pointers.h"
template <class T>
using ObjectPtr = Utility::ObjectPtr<T>;
template <class T>
using WeakPtr = Utility::WeakPtr<T>;
#include "Utility/Memory/Object.h"
template <class T>
using Object = Utility::Object<T>;

#include "Utility/Math/Vector/Vector.hpp"
#include "Utility/Math/Matrix/Matrix.hpp"
#include "Utility/Math/Quaternion.hpp"
typedef Utility::Math::Vector2<float> Vec2F;
typedef Utility::Math::Vector2<int> Vec2I;
typedef Utility::Math::Vector2<double> Vec2D;
typedef Utility::Math::Vector3<float> Vec3F;
typedef Utility::Math::Vector3<int> Vec3I;
typedef Utility::Math::Vector4<float> Vec4F;
typedef Utility::Math::Vector4<int> Vec4I;
typedef Utility::Math::Matrix3x3<float> Mat3F;
typedef Utility::Math::Matrix4x4<float> Mat4F;
typedef Utility::Math::Quaternion<float> QuatF;

#include "Utility/Color.h"
typedef Utility::Color Color;

namespace Utility
{
    namespace Math
    {
        class Random;
    }
}

typedef Utility::Math::Random Random; 

#include "Utility/Time/Timepoint.h"
#include "Utility/Time/Timer.h"
#include "Utility/Math/Hash.h"

#include "Utility/String/StringConversion.h"
#include "Utility/String/StringConversionMath.h"
#include "Utility/String/StringFunctions.h"
