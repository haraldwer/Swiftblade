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

			Quaternion() : w(static_cast<Type>(1.0)), x(DefaultInitializationValue<Type>()), y(DefaultInitializationValue<Type>()), z(DefaultInitializationValue<Type>()) {}
			Quaternion(const Quaternion& q) : w(q.w), x(q.x), y(q.y), z(q.z) {}
			Quaternion(float ax, float ay, float az, float aw) : w(aw), x(ax), y(ay), z(az) {}
			Quaternion(Type angle, const Vector3<Type>& axis)
			{
				w = cos(angle / 2);
				const Type sinAngle = sin(angle / 2);
				x = axis.x * sinAngle;
				y = axis.y * sinAngle;
				z = axis.z * sinAngle;
			}

			static Quaternion FromEuler(const Vector3<Type>& euler)
			{
				// Assuming the angles are in radians.
				const Type heading = euler.y;
				const Type attitude = euler.z;
				const Type bank = euler.x;

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

			static Quaternion FromDirection(const Vector3<Type>& direction)
			{
				const float angle = atan2(direction.z, direction.x); // Maybe switch
				Quaternion q; 
				q.x = 0; 
				q.y = 1 * sin(angle / 2); 
				q.z = 0;
				q.w = cos(angle / 2);
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
			
			Quaternion operator*(const Quaternion& q2) const
			{	
				Quaternion t;
				t.x = x * q2.w + y * q2.z - z * q2.y + w * q2.x;
				t.y = -x * q2.z + y * q2.w + z * q2.x + w * q2.y;
				t.z = x * q2.y - y * q2.x + z * q2.w + w * q2.z;
				t.w = -x * q2.x - y * q2.y - z * q2.z + w * q2.w;
				return t;
			}


			Quaternion operator/(float aFloat) const
			{
				Quaternion t;
				t.x = x / aFloat;
				t.y = y / aFloat;
				t.z = z / aFloat;
				t.w = w / aFloat;
				return t;
			}

			Quaternion operator/=(float aFloat)
			{
				x /= aFloat;
				y /= aFloat;
				z /= aFloat;
				w /= aFloat;
				return *this;
			}

			Quaternion operator*(float aFloat) const
			{
				Quaternion t;
				t.x = x * aFloat;
				t.y = y * aFloat;
				t.z = z * aFloat;
				t.w = w * aFloat;
				return t;
			}

			Quaternion operator*=(float aFloat)
			{
				x *= aFloat;
				y *= aFloat;
				z *= aFloat;
				w *= aFloat;
				return *this;
			}


			Quaternion operator+(const Quaternion& q2) const
			{
				Quaternion t;
				t.x += q2.x;
				t.y += q2.y;
				t.z += q2.z;
				t.w += q2.w;
				return t;
			}

			Quaternion operator+=(const Quaternion& q2) const
			{
				x += q2.x;
				y += q2.y;
				z += q2.z;
				w += q2.w;
				return *this;
			}

			bool operator==(const Quaternion& q2) const
			{
				return
					x == q2.x &&
					y == q2.y &&
					z == q2.z &&
					w == q2.w;
			}

			bool operator!=(const Quaternion& q2) const
			{
				return !(*this == q2); 
			}

			Type Norm() const
			{
				return SquareRoot(x * x + y * y + z * z + w * w);
			}


			void Normalize()
			{
				Type norm = Norm();
				x /= norm;
				y /= norm;
				z /= norm;
				w /= norm;
			}
			
			template <class F>
			static Quaternion Slerp(Quaternion qa, Quaternion qb, F t) {
				// quaternion to return
				qa.Normalize();
				qb.Normalize();
				Quaternion qm;
				// Calculate angle between them.
				double cosHalfTheta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;

				if (cosHalfTheta < 0.0f) {
					qb.w = -qb.w; qb.x = -qb.x; qb.y = -qb.y; qb.z = qb.z;
					cosHalfTheta = -cosHalfTheta;
				}

				// if qa=qb or qa=-qb then theta = 0 and we can return qa
				if (abs(cosHalfTheta) >= 1.0f) {
					qm.w = qa.w; qm.x = qa.x; qm.y = qa.y; qm.z = qa.z;
					return qm;
				}
				// Calculate temporary values.
				double halfTheta = acos(cosHalfTheta);
				double sinHalfTheta = SquareRoot(1.0f - cosHalfTheta * cosHalfTheta);
				// if theta = 180 degrees then result is not fully defined
				// we could rotate around any axis normal to qa or qb
				if (fabs(sinHalfTheta) < 0.00001f) { // fabs is floating point absolute
					qm.w = (qa.w * 0.5f + qb.w * 0.5f);
					qm.x = (qa.x * 0.5f + qb.x * 0.5f);
					qm.y = (qa.y * 0.5f + qb.y * 0.5f);
					qm.z = (qa.z * 0.5f + qb.z * 0.5f);
					return qm;
				}
				double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
				double ratioB = sin(t * halfTheta) / sinHalfTheta;
				//calculate Quaternion.
				qm.w = (qa.w * static_cast<Type>(ratioA) + qb.w * static_cast<Type>(ratioB));
				qm.x = (qa.x * static_cast<Type>(ratioA) + qb.x * static_cast<Type>(ratioB));
				qm.y = (qa.y * static_cast<Type>(ratioA) + qb.y * static_cast<Type>(ratioB));
				qm.z = (qa.z * static_cast<Type>(ratioA) + qb.z * static_cast<Type>(ratioB));
				qm.Normalize();
				return qm;
			}

			static Quaternion Identity() { return Quaternion(); }
			
		};
	}
}
