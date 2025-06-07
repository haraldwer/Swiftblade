#pragma once

#include "Macros.h"
#include "SqareRoot.h"

namespace Utility
{
	namespace Math
	{
		template <class Type>
		class Quaternion
		{
		public:

			union
			{
				Type data[4];

				struct
				{
					Type w;
					Type x;
					Type y;
					Type z;
				};
			};

			Quaternion& operator = (const Quaternion& InOther)
			{
				(*this).w = InOther.w;
				(*this).x = InOther.x;
				(*this).y = InOther.y;
				(*this).z = InOther.z;
				return *this;
			}

			Quaternion() : w(static_cast<Type>(1.0)), x(DefaultInitializationValue<Type>()), y(DefaultInitializationValue<Type>()), z(DefaultInitializationValue<Type>()) {}
			Quaternion(const Quaternion& InQuat) : w(InQuat.w), x(InQuat.x), y(InQuat.y), z(InQuat.z) {}
			Quaternion(float InW, float InX, float InY, float InZ) : w(InW), x(InX), y(InY), z(InZ) {}
			Quaternion(Type InAngle, const Vector3<Type>& InAxis)
			{
				w = cos(InAngle / 2);
				const Type sinAngle = sin(InAngle / 2);
				x = InAxis.x * sinAngle;
				y = InAxis.y * sinAngle;
				z = InAxis.z * sinAngle;
			}

			static Quaternion FromEuler(const Vector3<Type>& InEuler)
			{
				// Assuming the angles are in radians.
				const Type heading = InEuler.y;
				const Type attitude = InEuler.z;
				const Type bank = InEuler.x;

				const Type c1 = cos(heading / 2);
				const Type s1 = sin(heading / 2);
				const Type c2 = cos(attitude / 2);
				const Type s2 = sin(attitude / 2);
				const Type c3 = cos(bank / 2);
				const Type s3 = sin(bank / 2);
				const Type c1c2 = c1 * c2;
				const Type s1s2 = s1 * s2;
				
				Quaternion q; 
				q.w = c1c2 * c3 - s1s2 * s3;
				q.x = c1c2 * s3 + s1s2 * c3;
				q.y = s1 * c2 * c3 + c1 * s2 * s3;
				q.z = c1 * s2 * c3 - s1 * c2 * s3;
				return q; 
			}
			
			Vector3<Type> Euler() const
			{
				Type heading, attitude, bank;
				Type sqw = w * w;
				Type sqx = x * x;
				Type sqy = y * y;
				Type sqz = z * z;
				Type unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
				Type test = x * y + z * w;
				if (test > 0.499 * unit) { // singularity at north pole
					heading = 2 * atan2(x, w);
					attitude = PI_FLOAT / 2;
					bank = 0;
					return { bank, heading, attitude };
				}
				if (test < -0.499 * unit) { // singularity at south pole
					heading = -2 * atan2(x, w);
					attitude = -PI_FLOAT / 2;
					bank = 0;
					return { bank, heading, attitude };
				}
				heading = atan2(2 * y * w - 2 * x * z, sqx - sqy - sqz + sqw);
				attitude = asin(2 * test / unit);
				bank = atan2(2 * x * w - 2 * y * z, -sqx + sqy - sqz + sqw);
				return { bank, heading, attitude };
			}

			static Quaternion FromDirection(const Vector3<Type>& InDirection)
			{
				const float angle = atan2(InDirection.z, InDirection.x); // Maybe switch
				Quaternion q; 
				q.x = 0; 
				q.y = 1 * sin(angle / 2); 
				q.z = 0;
				q.w = cos(angle / 2);
				return q;
			}

			Type Length() const
			{
				return SquareRoot(x * x + y * y + z * z + w * w);
			}

			Quaternion GetNormalized()
			{
				Type l = Length();
				return *this / l;				
			}

			Quaternion& Normalize()
			{
				*this = GetNormalized();
				return *this;
			}

			static Type Dot(const Quaternion& InA, const Quaternion& InB)
			{
				return InA.w * InB.w + InA.x * InB.x + InA.y * InB.y + InA.z * InB.z;
			}
			
			template <class F>
			static Quaternion Slerp(Quaternion InA, Quaternion InB, F InT)
			{
				// Compute the dot product to check the angle between the quaternions
				float dot = Quaternion::Dot(InA, InB);

				// If the dot product is negative, slerp will take the long way around, so we negate one quaternion
				if (dot < static_cast<Type>(0.0f)) {
					InB *= static_cast<Type>(-1.0f);
					dot = -dot;
				}

				// If the dot product is close to 1, use linear interpolation to avoid division by zero
				const Type threshold = static_cast<Type>(0.9995f);
				if (dot > threshold) {
					// Linear interpolation (lerp)
					Quaternion result = InA + (InB - InA) * InT;
					result.Normalize();
					return result;
				}

				// Compute the angle between the quaternions
				const float theta0 = std::acos(dot);
				const float theta = theta0 * InT;

				// Compute the second quaternion
				const Quaternion q2New = (InB - InA * dot).GetNormalized();

				// Perform the slerp interpolation
				const Quaternion result = (InA * std::cos(theta) + q2New * std::sin(theta)).GetNormalized();
				return result;
			}
			
			static Quaternion Identity() { return Quaternion(); }
			
		};

		template <class Type>
		bool operator==(const Quaternion<Type>& InFirst, const Quaternion<Type>& InSecond)
		{
			return
				InFirst.x == InSecond.x &&
				InFirst.y == InSecond.y &&
				InFirst.z == InSecond.z &&
				InFirst.w == InSecond.w;
		}

		template <class Type>
		Quaternion<Type> operator * (const Quaternion<Type>& InFirst, const Quaternion<Type>& InSecond)
		{	
			Quaternion<Type> t;
			t.w = InFirst.w * InSecond.w - InFirst.x * InSecond.x - InFirst.y * InSecond.y - InFirst.z * InSecond.z;
			t.x = InFirst.w * InSecond.x + InFirst.x * InSecond.w + InFirst.y * InSecond.z - InFirst.z * InSecond.y;
			t.y = InFirst.w * InSecond.y - InFirst.x * InSecond.z + InFirst.y * InSecond.w + InFirst.z * InSecond.x;
			t.z = InFirst.w * InSecond.z + InFirst.x * InSecond.y - InFirst.y * InSecond.x + InFirst.z * InSecond.w;
			return t;
		}

		template <class Type>
		Quaternion<Type> operator / (const Quaternion<Type>& InFirst, Type InValue)
		{
			Quaternion<Type> t;
			t.w = InFirst.w / InValue;
			t.x = InFirst.x / InValue;
			t.y = InFirst.y / InValue;
			t.z = InFirst.z / InValue;
			return t;
		}

		template <class Type>
		Quaternion<Type>& operator /= (Quaternion<Type>& InFirst, Type InValue)
		{
			InFirst = InFirst / InValue;
			return InFirst;
		}

		template <class Type>
		Quaternion<Type> operator * (const Quaternion<Type>& InFirst, Type InValue)
		{
			Quaternion<Type> t;
			t.w = InFirst.w * InValue;
			t.x = InFirst.x * InValue;
			t.y = InFirst.y * InValue;
			t.z = InFirst.z * InValue;
			return t;
		}

		template <class Type>
		Quaternion<Type>& operator *= (Quaternion<Type>& InFirst, Type InValue)
		{
			InFirst = InFirst * InValue;
			return InFirst;
		}

		template <class Type>
		Quaternion<Type> operator + (const Quaternion<Type>& InFirst, const Quaternion<Type>& InSecond)
		{
			Quaternion<Type> t;
			t.w = InFirst.w + InSecond.w;
			t.x = InFirst.x + InSecond.x;
			t.y = InFirst.y + InSecond.y;
			t.z = InFirst.z + InSecond.z;
			return t;
		}

		template <class Type>
		Quaternion<Type>& operator += (Quaternion<Type>& InFirst, const Quaternion<Type>& InSecond)
		{
			InFirst = InFirst + InSecond;
			return InFirst;
		}

		template <class Type>
		Quaternion<Type> operator - (const Quaternion<Type>& InFirst, const Quaternion<Type>& InSecond)
		{
			Quaternion<Type> t;
			t.w = InFirst.w - InSecond.w;
			t.x = InFirst.x - InSecond.x;
			t.y = InFirst.y - InSecond.y;
			t.z = InFirst.z - InSecond.z;
			return t;
		}

		template <class Type>
		Quaternion<Type> operator -= (const Quaternion<Type>& InFirst, const Quaternion<Type>& InSecond)
		{
			InFirst = InFirst - InSecond;
			return InFirst;
		}
	}
}
