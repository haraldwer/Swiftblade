#pragma once

#include "Utility/Math/DefaultInitialization.h"

namespace Utility
{
	namespace Math
	{
		template<typename Type, int Alignment = 0>
		struct alignas(Alignment) Vector3
		{
			union
			{
				Type data[3];

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

						Vector2<Type> xy;
					};

					union
					{
						Type z;
						Type depth;
						Type blue;
						Type b;
					};
				};
			};

			Type LengthSqr() const
			{
				return x * x + y * y + z * z;
			}
			
			Type Length() const
			{
				return SquareRoot(LengthSqr());
			}

			Vector3 GetNormalized() const
			{
				const Type l = Length();
				if (l < SMALL_NUMBER)
					return Vector3::Zero();
				return (*this) / l;
			}

			Vector3& Normalize()
			{
				*this = GetNormalized();
				return *this;
			}

			static Type Dot(const Vector3& InA, const Vector3& InB)
			{
				return InA.x * InB.x + InA.y * InB.y + InA.z * InB.z;
			}

			static Vector3 Cross(const Vector3& InA, const Vector3& InB)
			{
				return Vector3(
					InA.y * InB.z - InA.z * InB.y,
					InA.z * InB.x - InA.x * InB.z,
					InA.x * InB.y - InA.y * InB.x);
			}

			template <class F>
			static Vector3 Lerp(const Vector3& InA, const Vector3& InB, const F InT)
			{
				return Math::Lerp(InA, InB, InT);
			}
			
			const Type& operator [](int index) const
			{
				return data[index];
			}

			Type& operator [](int index)
			{
				return data[index];
			}

			Vector3& operator = (const Vector3<Type>& InOther)
			{
				(*this).x = InOther.x;
				(*this).y = InOther.y;
				(*this).z = InOther.z;
				return *this;
			}

			template <class OtherType>
			Vector3<OtherType, Alignment> To() const
			{
				return {
					static_cast<OtherType>(x),
					static_cast<OtherType>(y),
					static_cast<OtherType>(z)
				};
			}
			
			Vector3(const Type& InX, const Type& InY, const Type& InZ) : data{ InX, InY, InZ } {}
			Vector3(const Type& InValue) : data{ InValue, InValue, InValue } {}
			Vector3() : data{ DefaultInitializationValue<Type>(), DefaultInitializationValue<Type>(), DefaultInitializationValue<Type>() } {}

			static Vector3 Up()			{ return { 0, 1, 0 }; }
			static Vector3 Down()		{ return { 0,-1, 0 }; }
			static Vector3 Right()		{ return { 1, 0, 0 }; }
			static Vector3 Left()		{ return {-1, 0, 0 }; }
			static Vector3 Forward()	{ return { 0, 0, 1 }; }
			static Vector3 Backward()	{ return { 0, 0,-1 }; }
			static Vector3 Zero()		{ return { 0, 0, 0 }; }
			static Vector3 One()		{ return { 1, 1, 1 }; }
		};

		
		template <class Type>
		bool operator==(const Vector3<Type>& InFirst, const Vector3<Type>& InSecond)
		{
			return
				InFirst.x == InSecond.x &&
				InFirst.y == InSecond.y &&
				InFirst.z == InSecond.z;
		}

		template <class Type>
		Vector3<Type> operator + (const Vector3<Type>& InFirst, const Vector3<Type>& InSecond)
		{
			return {
				InFirst.x + InSecond.x,
				InFirst.y + InSecond.y,
				InFirst.z + InSecond.z
			};
		}

		template <class Type>
		Vector3<Type>& operator += (Vector3<Type>& InFirst, const Vector3<Type>& InSecond)
		{
			InFirst = InFirst + InSecond;
			return InFirst;
		}

		template <class Type>
		Vector3<Type> operator - (const Vector3<Type>& InFirst, const Vector3<Type>& InSecond)
		{
			return {
				InFirst.x - InSecond.x,
				InFirst.y - InSecond.y,
				InFirst.z - InSecond.z
			};
		}

		template <class Type>
		Vector3<Type>& operator -= (Vector3<Type>& InFirst, const Vector3<Type>& InSecond)
		{
			InFirst = InFirst - InSecond;
			return InFirst;
		}

		template <class Type>
		Vector3<Type> operator * (const Vector3<Type>& InFirst, const Vector3<Type>& InSecond)
		{
			return {
				InFirst.x * InSecond.x,
				InFirst.y * InSecond.y,
				InFirst.z * InSecond.z
			};
		}
		
		template <class Type>
		Vector3<Type>& operator *= (Vector3<Type>& InFirst, const Vector3<Type>& InSecond)
		{
			InFirst = InFirst * InSecond;
			return InFirst;
		}

		template <class Type>
		Vector3<Type> operator * (const Vector3<Type>& InFirst, const float InSecond)
		{
			return {
				InFirst.x * InSecond,
				InFirst.y * InSecond,
				InFirst.z * InSecond
			};
		}

		template <class Type>
		Vector3<Type>& operator *= (Vector3<Type>& InFirst, const float InSecond)
		{
			InFirst = InFirst * InSecond;
			return InFirst;
		}

		template <class Type>
		Vector3<Type> operator * (const float InSecond, const Vector3<Type>& InFirst)
		{
			return {
				InFirst.x * InSecond,
				InFirst.y * InSecond,
				InFirst.z * InSecond
			};
		}
		
		template <class Type>
		Vector3<Type> operator / (const Vector3<Type>& InFirst, const Vector3<Type>& InSecond)
		{
			return {
				InFirst.x / InSecond.x,
				InFirst.y / InSecond.y,
				InFirst.z / InSecond.z
			};
		}

		template <class Type>
		Vector3<Type>& operator /= (Vector3<Type>& InFirst, const Vector3<Type>& InSecond)
		{
			InFirst = InFirst / InSecond;
			return InFirst;
		}

		template <class Type>
		Vector3<Type> operator / (const Vector3<Type>& InFirst, const float InSecond)
		{
			return {
				InFirst.x / InSecond,
				InFirst.y / InSecond,
				InFirst.z / InSecond
			};
		}

		template <class Type>
		Vector3<Type>& operator /= (Vector3<Type>& InFirst, const float InSecond)
		{
			InFirst = InFirst / InSecond;
			return InFirst;
		}
	}
}
