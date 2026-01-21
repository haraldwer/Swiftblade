#pragma once

#include "Utility/Math/DefaultInitialization.h"

namespace Utility
{
	namespace Math
	{
		template<typename Type, int Alignment = 0>
		struct alignas(Alignment) Vector4
		{
			union
			{
				Type data[4];

				struct
				{
					union
					{
						struct
						{
							union
							{
								struct
								{
									union
									{
										Type x;
										Type width;
										Type red;
										Type r;
									};

									union
									{
										Type y;
										Type height;
										Type green;
										Type g;
									};	
								};
								union
								{
									Vector2<Type> xy;
									Vector2<Type> rg;
								};
							};
							
							union
							{
								Type z;
								Type depth;
								Type blue;
								Type b;
							};
						};

						union
						{
							Vector3<Type> xyz;
							Vector3<Type> rgb;
						};
					};
					
					union
					{
						Type w;
						Type a;
					};
				};
			};

				
			Type LengthSqr() const
			{
				return x * x + y * y + z * z + w * w;
			}
			
			Type Length() const
			{
				return SquareRoot(LengthSqr());
			}

			Vector4 GetNormalized() const
			{
				const Type l = Length();
				if (l < SMALL_NUMBER)
					return Vector4::Zero();
				return (*this) / l;
			}

			Vector4& Normalize()
			{
				*this = GetNormalized();
				return *this;
			}

			static Type Dot(const Vector4& InA, const Vector4& InB)
			{
				return InA.x * InB.x + InA.y * InB.y + InA.z * InB.z + InA.w * InB.w;
			}

			template <class F>
			static Vector4 Lerp(const Vector4& InA, const Vector4& InB, const F InT)
			{
				return Math::Lerp(InA, InB, InT);
			}
			
			Type Min() const
			{
				return Math::Min(Math::Min(Math::Min(x, y), z), w);
			}
			
			Type Max() const
			{
				return Math::Max(Math::Max(Math::Max(x, y), z), w);
			}
			
			static Vector4 Min(const Vector4& InA, const Vector4& InB)
			{
				return {
					Math::Min(InA.x, InB.x),
					Math::Min(InA.y, InB.y),
					Math::Min(InA.z, InB.z),
					Math::Min(InA.w, InB.w)
				};
			}
			
			static Vector4 Max(const Vector4& InA, const Vector4& InB)
			{
				return {
					Math::Max(InA.x, InB.x),
					Math::Max(InA.y, InB.y),
					Math::Max(InA.z, InB.z),
					Math::Max(InA.w, InB.w)
				};
			}
			
			const Type& operator [](int InIndex) const
			{
				return data[InIndex];
			}

			Type& operator [](int InIndex)
			{
				return data[InIndex];
			}

			Vector4& operator = (const Vector4& InOther)
			{
				x = InOther.x;
				y = InOther.y;
				z = InOther.z;
				w = InOther.w;
				return (*this);
			}
			
			Vector4 operator + (const Vector4& InOther) const
			{
				return {
					x + InOther.x,
					y + InOther.y,
					z + InOther.z,
					w + InOther.w
				};
			}

			Vector4& operator += (const Vector4& InOther)
			{
				*this = *this + InOther;
				return *this;
			}
			
			Vector4 operator - (const Vector4& InOther) const
			{
				return {
					x - InOther.x,
					y - InOther.y,
					z - InOther.z,
					w - InOther.w
				};
			}

			Vector4& operator -= (const Vector4& InOther)
			{
				*this = *this - InOther;
				return *this;
			}

			Vector4 operator * (const Vector4& InOther) const
			{
				return {
					x * InOther.x,
					y * InOther.y,
					z * InOther.z,
					w * InOther.w
				};
			}

			Vector4& operator *= (const Vector4& InOther)
			{
				*this = *this * InOther;
				return *this;
			}
			
			Vector4 operator / (const Vector4& InOther) const 
			{
				return {
					x / InOther.x,
					y / InOther.y,
					z / InOther.z,
					w / InOther.w
				};
			}

			Vector4& operator /= (const Vector4& InOther)
			{
				*this = *this / InOther;
				return *this;
			}
			
			bool operator==(const Vector4& InOther) const
			{
				return
					x == InOther.x &&
					y == InOther.y &&
					z == InOther.z &&
					w == InOther.w;
			}

			template <class OtherType>
			Vector4<OtherType, Alignment> To() const
			{
				return {
					static_cast<OtherType>(x),
					static_cast<OtherType>(y),
					static_cast<OtherType>(z),
					static_cast<OtherType>(w)
				};
			}
				
			Vector4(const Type & InX, const Type & InY, const Type & InZ, const Type & InW) : data{ InX, InY, InZ, InW } {}
			Vector4(const Type & InValue) : data{ InValue, InValue, InValue, InValue } {}
			Vector4(const Vector2<Type>& InXY, const Vector2<Type>& InZW) : data{ InXY.x, InXY.y, InZW.x, InZW.y } {}
			explicit Vector4(const Vector3<Type>& InXYZ) : data{ InXYZ.x, InXYZ.y, InXYZ.z, 0 } {}
			explicit Vector4(const Vector2<Type>& InXY) : data{ InXY.x, InXY.y, 0, 0 } {}
			Vector4() : data{
				DefaultInitializationValue<Type>(),
				DefaultInitializationValue<Type>(),
				DefaultInitializationValue<Type>() ,
				DefaultInitializationValue<Type>() } {}

			static Vector4 Zero() { return { 0, 0, 0, 0 }; }
			static Vector4 One() { return { 1, 1, 1, 1 }; }
		};
	}
}
