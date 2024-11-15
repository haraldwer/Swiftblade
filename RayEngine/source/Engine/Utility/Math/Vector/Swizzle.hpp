
#pragma once
#include "..\..\Memory\Memory.h"
#include "..\..\Memory\TypeMap.h"

namespace Utility
{
	namespace Math
	{
		template<typename Type, int alignment = 0>
		struct Vector2;

		template<typename Type, int alignment = 0>
		struct Vector3;

		template<typename Type, int alignment = 0>
		struct Vector4;

		template<typename Type, int Offset1, int Offset2, int alignment = 0>
		struct Vector2Swizzle
		{

			union
			{
				Memory::Map<Type, (int)sizeof(Type) * Offset1> x;
				Memory::Map<Type, (int)sizeof(Type) * Offset2> y;

				struct
				{
					operator Vector2<Type, alignment> &()
					{
						return Memory::Cast<Vector2<Type, alignment> >(this, (-(int)sizeof(Type)));
					}
				}vector;
			};

			Vector2<Type, alignment>& operator = (const Vector2<Type, alignment> &aAssignment)
			{
				Type assignedX = aAssignment.x;
				Type assignedY = aAssignment.y;
				x = assignedX;
				y = assignedY;
				return Memory::Cast<Vector2<Type, alignment>>(this);
			}

			operator Vector2<Type, alignment>()
			{
				return Vector2<Type, alignment>(x, y);
			}

			Vector2<Type, alignment> operator ()()
			{
				return Vector2<Type, alignment>(x, y);
			}

			friend Vector2<Type, alignment>;
			friend Vector3<Type, alignment>;
			friend Vector4<Type, alignment>;

		private:
			Vector2Swizzle() {}
		};


		template<typename Type, int Offset1, int Offset2, int Offset3, int alignment = 0>
		struct Vector3Swizzle
		{
			union
			{
				Memory::Map<Type, (int)sizeof(Type) * Offset1> x;
				Memory::Map<Type, (int)sizeof(Type) * Offset2> y;
				Memory::Map<Type, (int)sizeof(Type) * Offset3> z;

				struct
				{
					operator Vector3<Type, alignment>& () const
					{
						return Memory::Cast<Vector3<Type, alignment> >(this, (-((int)sizeof(Type)*2)));
					}
				}vector;
			};

			Vector3<Type, alignment>& operator = (const Vector3<Type, alignment>& aAssignment)
			{
				Type assignedX = aAssignment.x;
				Type assignedY = aAssignment.y;
				Type assignedZ = aAssignment.z;
				x = assignedX;
				y = assignedY;
				z = assignedZ;
				return vector;
			}

			Vector3<Type, alignment>& operator = (const Type aAssignment)
			{
				x = aAssignment;
				y = aAssignment;
				z = aAssignment;
				return vector;
			}

			operator Vector3<Type, alignment>()  const
			{
				return Vector3<Type, alignment>(x, y, z);
			}

			Vector3<Type, alignment> operator ()() const
			{
				return Vector3<Type, alignment>(x, y, z);
			}

			friend Vector2<Type, alignment>;
			friend Vector3<Type, alignment>;
			friend Vector4<Type, alignment>;

		private:
			Vector3Swizzle() {}
		};


		template<typename Type, int Offset1, int Offset2, int Offset3, int Offset4, int alignment = 0>
		struct Vector4Swizzle
		{
			union
			{
				Memory::Map<Type, (int)sizeof(Type) * Offset1> x;
				Memory::Map<Type, (int)sizeof(Type) * Offset2> y;
				Memory::Map<Type, (int)sizeof(Type) * Offset3> z;
				Memory::Map<Type, (int)sizeof(Type) * Offset4> w;

				struct
				{
					operator Vector4<Type, alignment>& ()
					{
						return Memory::Cast<Vector4<Type, alignment> >(this, (-((int)sizeof(Type) * 3)));
					}
				}vector;
			};

			Vector4<Type, alignment>& operator = (const Vector4<Type, alignment>& aAssignment)
			{
				Type assignedX = aAssignment.x;
				Type assignedY = aAssignment.y;
				Type assignedZ = aAssignment.z;
				Type assignedW = aAssignment.w;
				x = assignedX;
				y = assignedY;
				z = assignedZ;
				w = assignedW;
				return Memory::Cast<Vector4<Type, alignment>>(this);
			}

			operator Vector4<Type, alignment>()
			{
				return Vector4<Type, alignment>(x, y, z);
			}

			Vector4<Type, alignment> operator ()()
			{
				return Vector4<Type, alignment>(x, y, z);
			}


			friend Vector2<Type, alignment>;
			friend Vector3<Type, alignment>;
			friend Vector4<Type, alignment>;

		private:
			Vector4Swizzle() {}
		};
	}
}
