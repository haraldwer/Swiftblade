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
