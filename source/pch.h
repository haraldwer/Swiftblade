
#pragma once

#include <set>
#include <array>
#include <vector>
#include <unordered_map>
#include <string>
template <class T>
using Set = std::set<T>;
template <class T>
using Vector = std::vector<T>;
template <class T, int Size>
using Array = std::array<T, Size>;
template <class Key, class Val>
using Map = std::unordered_map<Key, Val>;
using String = std::string; 

#include "Utility/Utility.h"
#include "Utility/NumericalTypes.h"

#include "Utility/Memory/Pointers.h"
template <class T>
using ObjectPtr = Utility::ObjectPtr<T>;
template <class T>
using WeakPtr = Utility::WeakPtr<T>;

#include "Utility/Math/Vector/Vector.hpp"
#include "Utility/Math/Matrix/Matrix.hpp"
#include "Utility/Math/Quaternion.hpp"
typedef Utility::Math::Vector2<float> Vec2F;
typedef Utility::Math::Vector3<float> Vec3F;
typedef Utility::Math::Vector4<float> Vec4F;
typedef Utility::Math::Matrix3x3<float> Mat3F;
typedef Utility::Math::Matrix4x4<float> Mat4F;
typedef Utility::Math::Quaternion<float> QuatF;

#include "Utility/JsonUtility.h"

