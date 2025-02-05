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

			Type& operator [](int aIndex)
			{
				return data[aIndex];
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
			
			bool operator==(const Vector2& in) const
			{
				return
					x == in.x &&
					y == in.y;
			}

			Vector2(const Type& a_x, const Type& a_y) : data{ a_x, a_y } {}
			Vector2(const Type& a_v) : data{ a_v, a_v } {}
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
