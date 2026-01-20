#pragma once

#include "Utility/Math/DefaultInitialization.h"
#include "Utility/Math/Lerp.h"
#include "Utility/Math/SqareRoot.h"

namespace Utility
{
	namespace Math
	{
		template<typename Type, int Alignment = 0>
		struct alignas(Alignment) Vector2
		{
			union
			{
				Type data[2];

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
			};
			

			Type LengthSqr() const
			{
				return x * x + y * y;
			}
			
			Type Length() const
			{
				return SquareRoot(LengthSqr());
			}

			Vector2 GetNormalized() const
			{
				const Type l = Length();
				if (l < SMALL_NUMBER)
					return Vector2::Zero();
				return (*this) / l;
			}

			Vector2& Normalize()
			{
				*this = GetNormalized();
				return *this;
			}

			static Type Dot(const Vector2& InA, const Vector2& InB)
			{
				return InA.x * InB.x + InA.y * InB.y;
			}

			static Type Cross(const Vector2& InA, const Vector2& InB)
			{
				return InA.x * InB.y - InA.y * InB.y;
			}	

			template <class F>
			static Vector2 Lerp(const Vector2& InA, const Vector2& InB, const F InT)
			{
				return Math::Lerp(InA, InB, InT);
			}
			
			Type Min() const
			{
				return Math::Min(x, y);
			}
			
			Type Max() const
			{
				return Math::Max(x, y);
			}

			static Vector2 Min(const Vector2& InA, const Vector2& InB)
			{
				return {
					Math::Min(InA.x, InB.x),
					Math::Min(InA.y, InB.y),
				};
			}
			
			static Vector2 Max(const Vector2& InA, const Vector2& InB)
			{
				return {
					Math::Max(InA.x, InB.x),
					Math::Max(InA.y, InB.y),
				};
			}
			
			Type& operator [](int InIndex)
			{
				return data[InIndex];
			}
			 
			Vector2& operator = (const Vector2& InOther)
			{
				x = InOther.x;
				y = InOther.y;
				return (*this);
			}

			Vector2 operator + (const Vector2& InOther) const
			{
				return {
					x + InOther.x,
					y + InOther.y
				};
			}

			Vector2& operator += (const Vector2& InOther)
			{
				*this = *this + InOther;
				return *this;
			}
			
			Vector2 operator - (const Vector2& InOther) const
			{
				return {
					x - InOther.x,
					y - InOther.y
				};
			}

			Vector2& operator -= (const Vector2& InOther)
			{
				*this = *this - InOther;
				return *this;
			}

			Vector2 operator * (const Vector2& InOther) const
			{
				return {
					x * InOther.x,
					y * InOther.y
				};
			}

			Vector2& operator *= (const Vector2& InOther)
			{
				*this = *this * InOther;
				return *this;
			}
			
			Vector2 operator / (const Vector2& InOther) const
			{
				return {
					x / InOther.x,
					y / InOther.y
				};
			}

			Vector2& operator /= (const Vector2& InOther)
			{
				*this = *this / InOther;
				return *this;
			}
			
			bool operator==(const Vector2& InOther) const
			{
				return
					x == InOther.x &&
					y == InOther.y;
			}

			template <class OtherType>
			Vector2<OtherType, Alignment> To() const
			{
				return {
					static_cast<OtherType>(x),
					static_cast<OtherType>(y)
				};
			}

			Vector2(const Type& InX, const Type& InY) : data{ InX, InY } {}
			Vector2(const Type& InV) : data{ InV, InV } {}
			Vector2() : data{ DefaultInitializationValue<Type>(), DefaultInitializationValue<Type>() } {}

			static Vector2 Up()			{ return { 0, 1 }; };
			static Vector2 Down()		{ return { 0,-1 }; };
			static Vector2 Forward()	{ return { 0, 1 }; };
			static Vector2 Backward()	{ return { 0,-1 }; };
			static Vector2 Right()		{ return { 1, 0 }; };
			static Vector2 Left()		{ return {-1, 0 }; };
			static Vector2 Zero()		{ return { 0, 0 }; };
			static Vector2 One()		{ return { 1, 1 }; };
		};
	}
}
