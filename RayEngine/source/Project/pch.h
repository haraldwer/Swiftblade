#pragma once

// Slim windows
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

// External libraries
#define RAYGUI_IMPLEMENTATION
#define IMGUI_IMPLEMENTATION
#define IMGUI_DISABLE_WIN32_FUNCTIONS
#define IMGUI_DEFINE_MATH_OPERATORS

#include <set>
#include <array>
#include <vector>
#include <unordered_map>
#include <string>

namespace Utility::Math
{
    class Random;
}

template <class T>
using Set = std::set<T>;
template <class T>
using Vector = std::vector<T>;
template <class T, int Size>
using Array = std::array<T, Size>;
template <class Key, class Val>
using Map = std::unordered_map<Key, Val>;
template <class First, class Second>
using Tuple = std::tuple<First, Second>;
using String = std::string; 

#include "Utility/Math/Macros.h"
#include "Utility/Math/Basic.h"
#include "Utility/Math/Lerp.h"
#include "Utility/Utility.h"
#include "Utility/NumericalTypes.h"

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
typedef Utility::Math::Vector3<float> Vec3F;
typedef Utility::Math::Vector4<float> Vec4F;
typedef Utility::Math::Matrix3x3<float> Mat3F;
typedef Utility::Math::Matrix4x4<float> Mat4F;
typedef Utility::Math::Quaternion<float> QuatF;
typedef Utility::Math::Random Random; 

#include "Utility/JsonUtility.h"
#include "Utility/Enum/BetterEnums.h"