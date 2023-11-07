
#pragma once

#include "../Vector/Vector4.hpp"
#include "../Vector/Vector3.hpp"
#include "../Quaternion.hpp"
#include "../SqareRoot.h"
#include "Utility/Math/Macros.h"

namespace Utility
{
	namespace Math
	{
		template<typename Type, int row_offset = 0, int column_offset = 0>
		class Matrix4x4
		{
		public:
			
			union
			{
				Type elements[4][4];
				Type data[16];

				struct
				{
					Vector4<Type> right;
					Vector4<Type> up;
					Vector4<Type> forward;
					Vector4<Type> translation;
				};
			};

			Matrix4x4()
			{
				elements[0][0] = 1.0f;
				elements[0][1] = 0.0f;
				elements[0][2] = 0.0f;
				elements[0][3] = 0.0f;

				elements[1][0] = 0.0f;
				elements[1][1] = 1.0f;
				elements[1][2] = 0.0f;
				elements[1][3] = 0.0f;

				elements[2][0] = 0.0f;
				elements[2][1] = 0.0f;
				elements[2][2] = 1.0f;
				elements[2][3] = 0.0f;

				elements[3][0] = 0.0f;
				elements[3][1] = 0.0f;
				elements[3][2] = 0.0f;
				elements[3][3] = 1.0f;
			}

			Matrix4x4(const Matrix4x4& aMatrix)
			{
				memcpy(data, aMatrix.data, sizeof(Type) * 16);
			}

			Matrix4x4(const Vector3<Type>& p, const Quaternion<Type>& q, const Vector3<Type>& s) : Matrix4x4()
			{
				SetPosition(p);
				SetScale(s);
				SetRotation(q);
			}

			Matrix4x4(const Vector3<Type>& p) : Matrix4x4()
			{
				SetPosition(p);
			}

			Matrix4x4(const Quaternion<Type>& q) : Matrix4x4()
			{
				SetRotation(q);
			}

			void Rotate(const Vector3<Type>& aRotation) 
			{
				Matrix4x4 n = GetNormalized();
				n *= CreateRotationAroundX(aRotation.x);
				n *= CreateRotationAroundY(aRotation.y);
				n *= CreateRotationAroundZ(aRotation.z);
				n.SetScale(Scale());
				*this = n;
			}

			static Matrix4x4 CreateRotationAroundAxis(Type aAngleInRadians, Vector3<Type> aNormalizedAxis)
			{
				Matrix4x4 new_matrix;
				Type cos_angle = cos(aAngleInRadians);
				Type sin_angle = sin(aAngleInRadians);

				Type u = aNormalizedAxis.x;
				Type v = aNormalizedAxis.y;
				Type w = aNormalizedAxis.z;

				new_matrix.elements[0][0] = u * u + (1.0f - u * u) * cos_angle;
				new_matrix.elements[0][1] = u * v * (1.0f - cos_angle) - w * sin_angle;
				new_matrix.elements[0][2] = u * w * (1.0f - cos_angle) + v * sin_angle;
				new_matrix.elements[0][3] = 0.0f;

				new_matrix.elements[1][0] = u * v * (1.0f - cos_angle) + w * sin_angle;
				new_matrix.elements[1][1] = v * v + (1.0f - v * v) * cos_angle;
				new_matrix.elements[1][2] = v * w * (1.0f - cos_angle) - u * sin_angle;
				new_matrix.elements[1][3] = 0.0f;

				new_matrix.elements[2][0] = u * w * (1.0f - cos_angle) - v * sin_angle;
				new_matrix.elements[2][1] = v * w * (1.0f - cos_angle) + u * sin_angle;
				new_matrix.elements[2][2] = w * w + (1.0f - w * w) * cos_angle;
				new_matrix.elements[2][3] = 0.0f;

				new_matrix.elements[3][0] = 0.0f;
				new_matrix.elements[3][1] = 0.0f;
				new_matrix.elements[3][2] = 0.0f;
				new_matrix.elements[3][3] = 1.0f;

				return new_matrix;
			}
			
			Vector3<Type> GetEuler() const
			{
				const Matrix4x4 n = GetNormalized();
				
				Type heading, attitude, bank;
			
				if (n.elements[0][1] > 0.998) { // singularity at north pole
					heading = atan2(n.elements[2][0], n.elements[2][2]);
					attitude = PI_FLOAT / 2;
					bank = 0;
					return { bank, heading, attitude };
				}
				if (n.elements[1][0] < -0.998) { // singularity at south pole
					heading = atan2(n.elements[2][0], n.elements[2][2]);
					attitude = -PI_FLOAT / 2;
					bank = 0;
					return { bank, heading, attitude };
				}
				heading = atan2(-n.elements[0][2], n.elements[0][0]);
				bank = atan2(-n.elements[2][1], n.elements[1][1]);
				attitude = asin(n.elements[0][1]);
				return { bank, heading, attitude };
			}

			void SetEuler(const Vector3<Type>& aRotation)
			{
				const Vector3<Type> scale = Scale();
				
				const Type heading = aRotation.y;
				const Type attitude = aRotation.z;
				const Type bank = aRotation.x;
			
				// Assuming the angles are in radians.
				Type ch = cos(heading);
				Type sh = sin(heading);
				Type ca = cos(attitude);
				Type sa = sin(attitude);
				Type cb = cos(bank);
				Type sb = sin(bank);
			
				elements[0][0] = ch * ca;
				elements[1][0] = sh * sb - ch * sa * cb;
				elements[2][0] = ch * sa * sb + sh * cb;
				elements[0][1] = sa;
				elements[1][1] = ca * cb;
				elements[2][1] = -ca * sb;
				elements[0][2] = -sh * ca;
				elements[1][2] = sh * sa * cb + ch * sb;
				elements[2][2] = -sh * sa * sb + ch * cb;
				
				SetScale(scale);
			}

			static Matrix4x4<Type> FromEuler(const Vector3<Type>& aRotation)
			{
				Matrix4x4 m;
				m.SetEuler(aRotation);
				return m;
			}
			
			void SetRotation(const Quaternion<Type>& q)
			{
				const Vector3<Type> p = GetPosition();
				const Vector3<Type> s = Scale();

				const Type sqw = q.w * q.w;
				const Type sqx = q.x * q.x;
				const Type sqy = q.y * q.y;
				const Type sqz = q.z * q.z;

				// Reset values outside the rot 3x3 part
				elements[0][3] = 0;
				elements[1][3] = 0;
				elements[2][3] = 0;
				elements[3][3] = 1;
				elements[3][0] = 0;
				elements[3][1] = 0;
				elements[3][2] = 0;

				// invs (inverse square length) is only required if quaternion is not already normalised
				const Type invs = static_cast<Type>(1.0) / (sqx + sqy + sqz + sqw);
				elements[0][0] = (sqx - sqy - sqz + sqw) * invs; // since sqw + sqx + sqy + sqz =1/invs*invs
				elements[1][1] = (-sqx + sqy - sqz + sqw) * invs;
				elements[2][2] = (-sqx - sqy + sqz + sqw) * invs;

				{
					const Type tmp1 = q.x * q.y;
					const Type tmp2 = q.z * q.w;
					elements[0][1] = static_cast<Type>(2.0) * (tmp1 + tmp2) * invs;
					elements[1][0] = static_cast<Type>(2.0) * (tmp1 - tmp2) * invs;
				}

				{
					const Type tmp1 = q.x * q.z;
					const Type tmp2 = q.y * q.w;
					elements[0][2] = static_cast<Type>(2.0) * (tmp1 - tmp2) * invs;
					elements[2][0] = static_cast<Type>(2.0) * (tmp1 + tmp2) * invs;
				}

				{
					const Type tmp1 = q.y * q.z;
					const Type tmp2 = q.x * q.w;
					elements[1][2] = static_cast<Type>(2.0) * (tmp1 + tmp2) * invs;
					elements[2][1] = static_cast<Type>(2.0) * (tmp1 - tmp2) * invs;
				}

				SetPosition(p);
				SetScale(s);
			}
			
			Matrix4x4 GetRotationMatrix() const
			{
				Matrix4x4 rotation;
				rotation.data[0] = data[0];
				rotation.data[1] = data[1];
				rotation.data[2] = data[2];
				rotation.data[4] = data[4];
				rotation.data[5] = data[5];
				rotation.data[6] = data[6];
				rotation.data[8] = data[8];
				rotation.data[9] = data[9];
				rotation.data[10] = data[10];
				return rotation;
			}

			Quaternion<Type> GetRotation() const
			{
				Quaternion<Type> q;
				const Type trelementsce = elements[0][0] + elements[1][1] + elements[2][2];
				if (trelementsce > 0) {
					const Type s = 0.5f / SquareRoot(trelementsce + 1.0f);
					q.w = 0.25f / s;
					q.x = (elements[1][2] - elements[2][1]) * s;
					q.y = (elements[2][0] - elements[0][2]) * s;
					q.z = (elements[0][1] - elements[1][0]) * s;
				}
				else {
					if (elements[0][0] > elements[1][1] && elements[0][0] > elements[2][2]) {
						const Type s = 2.0f * SquareRoot(1.0f + elements[0][0] - elements[1][1] - elements[2][2]);
						q.w = (elements[1][2] - elements[2][1]) / s;
						q.x = 0.25f * s;
						q.y = (elements[1][0] + elements[0][1]) / s;
						q.z = (elements[2][0] + elements[0][2]) / s;
					}
					else if (elements[1][1] > elements[2][2]) {
						const Type s = 2.0f * SquareRoot(1.0f + elements[1][1] - elements[0][0] - elements[2][2]);
						q.w = (elements[2][0] - elements[0][2]) / s;
						q.x = (elements[1][0] + elements[0][1]) / s;
						q.y = 0.25f * s;
						q.z = (elements[2][1] + elements[1][2]) / s;
					}
					else {
						const Type s = 2.0f * SquareRoot(1.0f + elements[2][2] - elements[0][0] - elements[1][1]);
						q.w = (elements[0][1] - elements[1][0]) / s;
						q.x = (elements[2][0] + elements[0][2]) / s;
						q.y = (elements[2][1] + elements[1][2]) / s;
						q.z = 0.25f * s;
					}
				}
				return q;
			}

			Vector3<Type> GetPosition() const
			{
				return { 
					elements[3][0], 
					elements[3][1], 
					elements[3][2] 
				};
			}

			void SetPosition(const Vector3<Type>& aPosition)
			{
				elements[3][0] = aPosition.x;
				elements[3][1] = aPosition.y;
				elements[3][2] = aPosition.z;
			}

			Type& operator()(const int aRow, const int aColumn)
			{
				return elements[(aRow - row_offset)][(aColumn - column_offset)];
			}

			const Type& operator()(const int aRow, const int aColumn) const
			{
				return elements[(aRow - row_offset)][(aColumn - column_offset)];
			}

			Matrix4x4 operator + (const Matrix4x4& aMatrix) const
			{
				Matrix4x4 new_matrix;
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						new_matrix.elements[row][column] = elements[row][column] + aMatrix.elements[row][column];
					}
				}
				return new_matrix;
			}

			Matrix4x4& operator += (const Matrix4x4& aMatrix)
			{
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						elements[row][column] += aMatrix.elements[row][column];
					}
				}
				return *this;
			}


			Matrix4x4 operator - (const Matrix4x4& aMatrix) const
			{
				Matrix4x4 new_matrix;
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						new_matrix.elements[row][column] = elements[row][column] - aMatrix.elements[row][column];
					}
				}
				return new_matrix;
			}

			Matrix4x4& operator -= (const Matrix4x4& aMatrix)
			{
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						elements[row][column] -= aMatrix.elements[row][column];
					}
				}
				return *this;
			}

			Matrix4x4& operator *= (const Matrix4x4& aMatrix)
			{
				return ((*this) = (*this) * aMatrix);
			}

			Matrix4x4& operator = (const Matrix4x4& aMatrix)
			{
				memcpy(data, aMatrix.data, sizeof(Type) * 16);
				return *this;
			}

			bool operator == (const Matrix4x4& aMatrix) const
			{
				for (int index = 0; index < 16; ++index)
				{
					if (data[index] != aMatrix.data[index])
					{
						return false;
					}
				}
				return true;
			}

			Matrix4x4 operator * (float aScalar) const
			{
				Matrix4x4 new_matrix;
				for (unsigned int i = 0; i < 16; i++)
					new_matrix.data[i] = data[i] * aScalar;
				return new_matrix;
			}

			Type ScaleX() const
			{
				return right.length;
			}
			
			Type ScaleY() const
			{
				return up.length;
			}

			Type ScaleZ() const
			{
				return forward.length;
			}

			Vector3<Type> Scale() const
			{
				return { ScaleX(), ScaleY(), ScaleZ() };
			}
			
			void SetScale(const Vector3<Type>& s)
			{
				right = right.GetNormalized() * s.x;
				up = up.GetNormalized() * s.y;
				forward = forward.GetNormalized() * s.z;
			}
			
			Vector3<Type> Forward() const
			{
				return Vector3<Type>(forward.xyz).GetNormalized();
			}
			
			Vector3<Type> Right() const
			{
				return Vector3<Type>(right.xyz).GetNormalized();
			}
			
			Vector3<Type> Up() const
			{
				return Vector3<Type>(up.xyz).GetNormalized();
			}
			
			Vector3<Type> TransformPoint(const Vector3<Type>& aPoint) const
			{
				Vector3<Type> result;
				result.x = aPoint.x * data[0] + aPoint.y * data[4] + aPoint.z * data[8] + data[12];
				result.y = aPoint.x * data[1] + aPoint.y * data[5] + aPoint.z * data[9] + data[13];
				result.z = aPoint.x * data[2] + aPoint.y * data[6] + aPoint.z * data[10] + data[14];
				return result;
			}
			
			Matrix4x4 GetNormalized() const
			{
				// Divide by scale
				auto m = *this;
				m.right.Normalize();
				m.up.Normalize();
				m.forward.Normalize();
				return m;
			}
			
			Matrix4x4& Normalize()
			{
				*this = GetNormalized();
				return *this; 
			}

			static Matrix4x4 CreateProjection(Type aAspectRatio, Type aFov, Type aNear, Type aFar)
			{
				Matrix4x4 new_matrix;
				new_matrix.elements[0][0] = (1.0f / aAspectRatio) * (1.0f / (tan(aFov / 2.0f)));
				new_matrix.elements[0][1] = 0.0f;
				new_matrix.elements[0][2] = 0.0f;
				new_matrix.elements[0][3] = 0.0f;
				
				new_matrix.elements[1][0] = 0.0f;
				new_matrix.elements[1][1] = -(1.0f / tan(aFov / 2.0f));
				new_matrix.elements[1][2] = 0.0f;
				new_matrix.elements[1][3] = 0.0f;
				
				new_matrix.elements[2][0] = 0.0f;
				new_matrix.elements[2][1] = 0.0f;
				new_matrix.elements[2][2] = aFar / (aFar - aNear);
				new_matrix.elements[2][3] = 1.0f;
				
				new_matrix.elements[3][0] = 0.0f;
				new_matrix.elements[3][1] = 0.0f;
				new_matrix.elements[3][2] = (-(aNear * aFar)) / (aFar - aNear);
				new_matrix.elements[3][3] = 0.0f;
				return new_matrix;
			}
			
			// Static functions for creating rotation matrices.
			static Matrix4x4 CreateRotationAroundX(Type aAngleInRadians)
			{
				Matrix4x4 new_matrix;
				Type cos_angle = cos(aAngleInRadians);
				Type sin_angle = sin(aAngleInRadians);

				new_matrix.elements[0][0] = 1.0f;
				new_matrix.elements[0][1] = 0.0f;
				new_matrix.elements[0][2] = 0.0f;
				new_matrix.elements[0][3] = 0.0f;

				new_matrix.elements[1][0] = 0.0f;
				new_matrix.elements[1][1] = cos_angle;
				new_matrix.elements[1][2] = sin_angle;
				new_matrix.elements[1][3] = 0.0f;

				new_matrix.elements[2][0] = 0.0f;
				new_matrix.elements[2][1] = -sin_angle;
				new_matrix.elements[2][2] = cos_angle;
				new_matrix.elements[2][3] = 0.0f;

				new_matrix.elements[3][0] = 0.0f;
				new_matrix.elements[3][1] = 0.0f;
				new_matrix.elements[3][2] = 0.0f;
				new_matrix.elements[3][3] = 1.0f;

				return new_matrix;
			}

			static Matrix4x4 CreateRotationAroundY(Type aAngleInRadians)
			{
				Matrix4x4 new_matrix;
				Type cos_angle = cos(aAngleInRadians);
				Type sin_angle = sin(aAngleInRadians);

				new_matrix.elements[0][0] = cos_angle;
				new_matrix.elements[0][1] = 0.0f;
				new_matrix.elements[0][2] = -sin_angle;
				new_matrix.elements[0][3] = 0.0f;

				new_matrix.elements[1][0] = 0.0f;
				new_matrix.elements[1][1] = 1.0;
				new_matrix.elements[1][2] = 0.0;
				new_matrix.elements[1][3] = 0.0f;

				new_matrix.elements[2][0] = sin_angle;
				new_matrix.elements[2][1] = 0.0;
				new_matrix.elements[2][2] = cos_angle;
				new_matrix.elements[2][3] = 0.0f;

				new_matrix.elements[3][0] = 0.0f;
				new_matrix.elements[3][1] = 0.0f;
				new_matrix.elements[3][2] = 0.0f;
				new_matrix.elements[3][3] = 1.0f;

				return new_matrix;
			}

			static Matrix4x4 CreateRotationAroundZ(Type aAngleInRadians)
			{
				Matrix4x4 new_matrix;
				Type cos_angle = cos(aAngleInRadians);
				Type sin_angle = sin(aAngleInRadians);

				new_matrix.elements[0][0] = cos_angle;
				new_matrix.elements[0][1] = sin_angle;
				new_matrix.elements[0][2] = 0.0f;
				new_matrix.elements[0][3] = 0.0f;

				new_matrix.elements[1][0] = -sin_angle;
				new_matrix.elements[1][1] = cos_angle;
				new_matrix.elements[1][2] = 0.0f;
				new_matrix.elements[1][3] = 0.0f;

				new_matrix.elements[2][0] = 0.0f;
				new_matrix.elements[2][1] = 0.0f;
				new_matrix.elements[2][2] = 1.0f;
				new_matrix.elements[2][3] = 0.0f;

				new_matrix.elements[3][0] = 0.0f;
				new_matrix.elements[3][1] = 0.0f;
				new_matrix.elements[3][2] = 0.0f;
				new_matrix.elements[3][3] = 1.0f;

				return new_matrix;
			}

			static Matrix4x4 Transpose(const Matrix4x4& aMatrix)
			{
				Matrix4x4 new_matrix;
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						new_matrix.elements[row][column] = aMatrix.elements[column][row];
					}
				}
				return new_matrix;
			}

			static Matrix4x4 GetFastInverse(const Matrix4x4& aMatrix)
			{
				Matrix4x4 new_matrix;
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						new_matrix.elements[row][column] = aMatrix.elements[column][row];
					}
				}

				for (unsigned int column = 0; column < 3; ++column)
				{
					Type total = 0.0f;
					total += -aMatrix.elements[3][0] * new_matrix.elements[0][column];
					total += -aMatrix.elements[3][1] * new_matrix.elements[1][column];
					total += -aMatrix.elements[3][2] * new_matrix.elements[2][column];
					new_matrix.elements[3][column] = total;
				}

				new_matrix.elements[3][3] = 1.0f;
				return new_matrix;
			}

			static Matrix4x4 GetInverse(const Matrix4x4& aMatrix)
			{
				const Type* const m = aMatrix.data;
				Matrix4x4 invMat;
				Type* inv = invMat.data;
				Type det;

				inv[0] = m[5] * m[10] * m[15] -
					m[5] * m[11] * m[14] -
					m[9] * m[6] * m[15] +
					m[9] * m[7] * m[14] +
					m[13] * m[6] * m[11] -
					m[13] * m[7] * m[10];

				inv[4] = -m[4] * m[10] * m[15] +
					m[4] * m[11] * m[14] +
					m[8] * m[6] * m[15] -
					m[8] * m[7] * m[14] -
					m[12] * m[6] * m[11] +
					m[12] * m[7] * m[10];

				inv[8] = m[4] * m[9] * m[15] -
					m[4] * m[11] * m[13] -
					m[8] * m[5] * m[15] +
					m[8] * m[7] * m[13] +
					m[12] * m[5] * m[11] -
					m[12] * m[7] * m[9];

				inv[12] = -m[4] * m[9] * m[14] +
					m[4] * m[10] * m[13] +
					m[8] * m[5] * m[14] -
					m[8] * m[6] * m[13] -
					m[12] * m[5] * m[10] +
					m[12] * m[6] * m[9];

				inv[1] = -m[1] * m[10] * m[15] +
					m[1] * m[11] * m[14] +
					m[9] * m[2] * m[15] -
					m[9] * m[3] * m[14] -
					m[13] * m[2] * m[11] +
					m[13] * m[3] * m[10];

				inv[5] = m[0] * m[10] * m[15] -
					m[0] * m[11] * m[14] -
					m[8] * m[2] * m[15] +
					m[8] * m[3] * m[14] +
					m[12] * m[2] * m[11] -
					m[12] * m[3] * m[10];

				inv[9] = -m[0] * m[9] * m[15] +
					m[0] * m[11] * m[13] +
					m[8] * m[1] * m[15] -
					m[8] * m[3] * m[13] -
					m[12] * m[1] * m[11] +
					m[12] * m[3] * m[9];

				inv[13] = m[0] * m[9] * m[14] -
					m[0] * m[10] * m[13] -
					m[8] * m[1] * m[14] +
					m[8] * m[2] * m[13] +
					m[12] * m[1] * m[10] -
					m[12] * m[2] * m[9];

				inv[2] = m[1] * m[6] * m[15] -
					m[1] * m[7] * m[14] -
					m[5] * m[2] * m[15] +
					m[5] * m[3] * m[14] +
					m[13] * m[2] * m[7] -
					m[13] * m[3] * m[6];

				inv[6] = -m[0] * m[6] * m[15] +
					m[0] * m[7] * m[14] +
					m[4] * m[2] * m[15] -
					m[4] * m[3] * m[14] -
					m[12] * m[2] * m[7] +
					m[12] * m[3] * m[6];

				inv[10] = m[0] * m[5] * m[15] -
					m[0] * m[7] * m[13] -
					m[4] * m[1] * m[15] +
					m[4] * m[3] * m[13] +
					m[12] * m[1] * m[7] -
					m[12] * m[3] * m[5];

				inv[14] = -m[0] * m[5] * m[14] +
					m[0] * m[6] * m[13] +
					m[4] * m[1] * m[14] -
					m[4] * m[2] * m[13] -
					m[12] * m[1] * m[6] +
					m[12] * m[2] * m[5];

				inv[3] = -m[1] * m[6] * m[11] +
					m[1] * m[7] * m[10] +
					m[5] * m[2] * m[11] -
					m[5] * m[3] * m[10] -
					m[9] * m[2] * m[7] +
					m[9] * m[3] * m[6];

				inv[7] = m[0] * m[6] * m[11] -
					m[0] * m[7] * m[10] -
					m[4] * m[2] * m[11] +
					m[4] * m[3] * m[10] +
					m[8] * m[2] * m[7] -
					m[8] * m[3] * m[6];

				inv[11] = -m[0] * m[5] * m[11] +
					m[0] * m[7] * m[9] +
					m[4] * m[1] * m[11] -
					m[4] * m[3] * m[9] -
					m[8] * m[1] * m[7] +
					m[8] * m[3] * m[5];

				inv[15] = m[0] * m[5] * m[10] -
					m[0] * m[6] * m[9] -
					m[4] * m[1] * m[10] +
					m[4] * m[2] * m[9] +
					m[8] * m[1] * m[6] -
					m[8] * m[2] * m[5];

				det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

				if (det == 0)
				{
					return Matrix4x4();
				}

				det = 1.0 / det;

				for(size_t i = 0; i < 16; i++)
				{
					inv[i] *= det;
				}
				
				return invMat;
			}

			Matrix4x4 operator*(const Matrix4x4& aMatrix) const
			{
				Matrix4x4 new_matrix;
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						Type total = 0.0f;
						for (int index = 0; index < 4; ++index)
						{
							total += elements[row][index] * aMatrix.elements[index][column];
						}
	
						new_matrix.elements[row][column] = total;
					}
				}
				return new_matrix;
			}
		};

		template<typename Type, int row_offset, int column_offset>
		Vector4<Type> operator * (Vector4<Type> aVector, Matrix4x4<Type, row_offset, column_offset> aMatrix)
		{
			Vector4<Type> new_vector;
			for (unsigned int column = 0; column < 4; ++column)
			{
				Type total = 0.0f;
				total += aVector.x * aMatrix.elements[0][column];
				total += aVector.y * aMatrix.elements[1][column];
				total += aVector.z * aMatrix.elements[2][column];
				total += aVector.w * aMatrix.elements[3][column];
				new_vector[column] = total;
			}

			return new_vector;
		}

		template<typename Type, int row_offset, int column_offset>
		Vector4<Type> operator * (Matrix4x4<Type, row_offset, column_offset> aMatrix, Vector4<Type> aVector)
		{
			Vector4<Type> new_vector;
			for (unsigned int column = 0; column < 4; ++column)
			{
				Type total = 0.0f;
				total += aMatrix.elements[0][column] * aVector.x;
				total += aMatrix.elements[1][column] * aVector.y;
				total += aMatrix.elements[2][column] * aVector.z;
				total += aMatrix.elements[3][column] * aVector.w;
				new_vector[column] = total;
			}

			return new_vector;
		}

	}
}
