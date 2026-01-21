
#pragma once

#include "Matrix3x3.hpp"
#include "../Vector/Vector4.hpp"
#include "../Vector/Vector3.hpp"
#include "../Quaternion.hpp"
#include "../SqareRoot.h"
#include "Utility/Math/Macros.h"

namespace Utility
{
	namespace Math
	{
		template<typename Type, int RowOffset = 0, int ColumnOffset = 0>
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

			Matrix4x4(const Matrix4x4& InMatrix)
			{
				data[0] = InMatrix.data[0];
				data[1] = InMatrix.data[1];
				data[2] = InMatrix.data[2];
				data[3] = InMatrix.data[3];
				data[4] = InMatrix.data[4];
				data[5] = InMatrix.data[5];
				data[6] = InMatrix.data[6];
				data[7] = InMatrix.data[7];
				data[8] = InMatrix.data[8];
				data[9] = InMatrix.data[9];
				data[10] = InMatrix.data[10];
				data[11] = InMatrix.data[11];
				data[12] = InMatrix.data[12];
				data[13] = InMatrix.data[13];
				data[14] = InMatrix.data[14];
				data[15] = InMatrix.data[15];
			}

			Matrix4x4(const Matrix3x3<Type, RowOffset, ColumnOffset>& InMatrix) : Matrix4x4()
			{
				elements[0][0] = InMatrix.elements[0][0];
				elements[1][0] = InMatrix.elements[1][0];
				elements[2][0] = InMatrix.elements[2][0];
				elements[0][1] = InMatrix.elements[0][1];
				elements[1][1] = InMatrix.elements[1][1];
				elements[2][1] = InMatrix.elements[2][1];
				elements[0][2] = InMatrix.elements[0][2];
				elements[1][2] = InMatrix.elements[1][2];
				elements[2][2] = InMatrix.elements[2][2];
			}

			Matrix4x4(const Vector3<Type>& InP, const Quaternion<Type>& InQ, const Vector3<Type>& InS) : Matrix4x4()
			{
				SetPosition(InP);
				SetRotation(InQ);
				SetScale(InS);
			}

			Matrix4x4(const Vector3<Type>& InP) : Matrix4x4()
			{
				SetPosition(InP);
			}

			Matrix4x4(const Quaternion<Type>& InQ) : Matrix4x4()
			{
				SetRotation(InQ);
			}

			void Rotate(const Vector3<Type>& InRot) 
			{
				Matrix4x4 n = GetNormalized();
				n *= CreateRotationAroundX(InRot.x);
				n *= CreateRotationAroundY(InRot.y);
				n *= CreateRotationAroundZ(InRot.z);
				n.SetScale(GetScale());
				*this = n;
			}

			static Matrix4x4 CreateRotationAroundAxis(Type InAngleRad, Vector3<Type> InNormAxis)
			{
				Matrix4x4 newMatrix;
				Type cosAngle = cos(InAngleRad);
				Type sinAngle = sin(InAngleRad);

				Type u = InNormAxis.x;
				Type v = InNormAxis.y;
				Type w = InNormAxis.z;

				newMatrix.elements[0][0] = u * u + (1.0f - u * u) * cosAngle;
				newMatrix.elements[0][1] = u * v * (1.0f - cosAngle) - w * sinAngle;
				newMatrix.elements[0][2] = u * w * (1.0f - cosAngle) + v * sinAngle;
				newMatrix.elements[0][3] = 0.0f;

				newMatrix.elements[1][0] = u * v * (1.0f - cosAngle) + w * sinAngle;
				newMatrix.elements[1][1] = v * v + (1.0f - v * v) * cosAngle;
				newMatrix.elements[1][2] = v * w * (1.0f - cosAngle) - u * sinAngle;
				newMatrix.elements[1][3] = 0.0f;

				newMatrix.elements[2][0] = u * w * (1.0f - cosAngle) - v * sinAngle;
				newMatrix.elements[2][1] = v * w * (1.0f - cosAngle) + u * sinAngle;
				newMatrix.elements[2][2] = w * w + (1.0f - w * w) * cosAngle;
				newMatrix.elements[2][3] = 0.0f;

				newMatrix.elements[3][0] = 0.0f;
				newMatrix.elements[3][1] = 0.0f;
				newMatrix.elements[3][2] = 0.0f;
				newMatrix.elements[3][3] = 1.0f;

				return newMatrix;
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

			void SetEuler(const Vector3<Type>& InRot)
			{
				const Vector3<Type> scale = GetScale();
				
				const Type heading = InRot.y;
				const Type attitude = InRot.z;
				const Type bank = InRot.x;
			
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

			static Matrix4x4<Type> FromEuler(const Vector3<Type>& InRot)
			{
				Matrix4x4 m;
				m.SetEuler(InRot);
				return m;
			}
			
			void SetRotation(const Quaternion<Type>& InQuat)
			{
				const Vector3<Type> p = GetPosition();
				const Vector3<Type> s = GetScale();

				const Type sqw = InQuat.w * InQuat.w;
				const Type sqx = InQuat.x * InQuat.x;
				const Type sqy = InQuat.y * InQuat.y;
				const Type sqz = InQuat.z * InQuat.z;

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
					const Type tmp1 = InQuat.x * InQuat.y;
					const Type tmp2 = InQuat.z * InQuat.w;
					elements[0][1] = static_cast<Type>(2.0) * (tmp1 + tmp2) * invs;
					elements[1][0] = static_cast<Type>(2.0) * (tmp1 - tmp2) * invs;
				}

				{
					const Type tmp1 = InQuat.x * InQuat.z;
					const Type tmp2 = InQuat.y * InQuat.w;
					elements[0][2] = static_cast<Type>(2.0) * (tmp1 - tmp2) * invs;
					elements[2][0] = static_cast<Type>(2.0) * (tmp1 + tmp2) * invs;
				}

				{
					const Type tmp1 = InQuat.y * InQuat.z;
					const Type tmp2 = InQuat.x * InQuat.w;
					elements[1][2] = static_cast<Type>(2.0) * (tmp1 + tmp2) * invs;
					elements[2][1] = static_cast<Type>(2.0) * (tmp1 - tmp2) * invs;
				}

				SetPosition(p);
				SetScale(s);
			}
			
			Matrix3x3<Type, RowOffset, ColumnOffset> GetRotationMatrix() const
			{
				Matrix3x3<Type, RowOffset, ColumnOffset> rotation;
				rotation.elements[0][0] = elements[0][0];
				rotation.elements[1][0] = elements[1][0];
				rotation.elements[2][0] = elements[2][0];
				rotation.elements[0][1] = elements[0][1];
				rotation.elements[1][1] = elements[1][1];
				rotation.elements[2][1] = elements[2][1];
				rotation.elements[0][2] = elements[0][2];
				rotation.elements[1][2] = elements[1][2];
				rotation.elements[2][2] = elements[2][2];
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

			void SetPosition(const Vector3<Type>& InPos)
			{
				elements[3][0] = InPos.x;
				elements[3][1] = InPos.y;
				elements[3][2] = InPos.z;
			}

			template <class F>
			static Matrix4x4 Lerp(const Matrix4x4& InA, const Matrix4x4& InB, F InT)
			{
				Vector3<Type> aP = InA.GetPosition();
				Vector3<Type> bP = InB.GetPosition();
				Vector3<Type> lP = Vector3<Type>::Lerp(aP, bP, InT);

				Vector3<Type> aS = InA.GetScale();
				Vector3<Type> bS = InB.GetScale();
				Vector3<Type> lS = Vector3<Type>::Lerp(aS, bS, InT);
				
				Quaternion<Type> aR = InA.GetRotation();
				Quaternion<Type> bR = InB.GetRotation();
				Quaternion<Type> lR = Quaternion<Type>::Slerp(aR, bR, InT);

				return Matrix4x4(lP, lR, lS);
			}
			
			Type& operator()(const int InRow, const int InColumn)
			{
				return elements[(InRow - RowOffset)][(InColumn - ColumnOffset)];
			}

			const Type& operator()(const int InRow, const int InColumn) const
			{
				return elements[(InRow - RowOffset)][(InColumn - ColumnOffset)];
			}

			Matrix4x4 operator + (const Matrix4x4& InMatrix) const
			{
				Matrix4x4 newMatrix;
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						newMatrix.elements[row][column] = elements[row][column] + InMatrix.elements[row][column];
					}
				}
				return newMatrix;
			}

			Matrix4x4& operator += (const Matrix4x4& InMatrix)
			{
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						elements[row][column] += InMatrix.elements[row][column];
					}
				}
				return *this;
			}


			Matrix4x4 operator - (const Matrix4x4& InMatrix) const
			{
				Matrix4x4 newMatrix;
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						newMatrix.elements[row][column] = elements[row][column] - InMatrix.elements[row][column];
					}
				}
				return newMatrix;
			}

			Matrix4x4& operator -= (const Matrix4x4& InMatrix)
			{
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						elements[row][column] -= InMatrix.elements[row][column];
					}
				}
				return *this;
			}

			Matrix4x4& operator *= (const Matrix4x4& InMatrix)
			{
				return ((*this) = (*this) * InMatrix);
			}

			Matrix4x4& operator = (const Matrix4x4& InMatrix)
			{
				data[0] = InMatrix.data[0];
				data[1] = InMatrix.data[1];
				data[2] = InMatrix.data[2];
				data[3] = InMatrix.data[3];
				data[4] = InMatrix.data[4];
				data[5] = InMatrix.data[5];
				data[6] = InMatrix.data[6];
				data[7] = InMatrix.data[7];
				data[8] = InMatrix.data[8];
				data[9] = InMatrix.data[9];
				data[10] = InMatrix.data[10];
				data[11] = InMatrix.data[11];
				data[12] = InMatrix.data[12];
				data[13] = InMatrix.data[13];
				data[14] = InMatrix.data[14];
				data[15] = InMatrix.data[15];
				return *this;
			}

			bool operator == (const Matrix4x4& InMatrix) const
			{
				for (int index = 0; index < 16; ++index)
				{
					if (data[index] != InMatrix.data[index])
					{
						return false;
					}
				}
				return true;
			}

			Matrix4x4 operator * (float InScalar) const
			{
				Matrix4x4 newMatrix;
				for (unsigned int i = 0; i < 16; i++)
					newMatrix.data[i] = data[i] * InScalar;
				return newMatrix;
			}

			Type ScaleX() const
			{
				return Vector3<Type>(right.x, right.y, right.z).Length();
			}
			
			Type ScaleY() const
			{
				return Vector3<Type>(up.x, up.y, up.z).Length();
			}

			Type ScaleZ() const
			{
				return Vector3<Type>(forward.x, forward.y, forward.z).Length();
			}

			Vector3<Type> GetScale() const
			{
				return { ScaleX(), ScaleY(), ScaleZ() };
			}
			
			void SetScale(const Vector3<Type>& InScale)
			{
				right = right.GetNormalized() * InScale.x;
				up = up.GetNormalized() * InScale.y;
				forward = forward.GetNormalized() * InScale.z;
			}
			
			Vector3<Type> Forward() const
			{
				return forward.xyz.GetNormalized();
			}
			
			Vector3<Type> Right() const
			{
				return right.xyz.GetNormalized();
			}
			
			Vector3<Type> Up() const
			{
				return up.xyz.GetNormalized();
			}
			
			Vector3<Type> TransformPoint(const Vector3<Type>& InPoint) const
			{
				Vector3<Type> result;
				result.x = InPoint.x * data[0] + InPoint.y * data[4] + InPoint.z * data[8] + data[12];
				result.y = InPoint.x * data[1] + InPoint.y * data[5] + InPoint.z * data[9] + data[13];
				result.z = InPoint.x * data[2] + InPoint.y * data[6] + InPoint.z * data[10] + data[14];
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

			static Matrix4x4 CreateProjection(Type InAspect, Type InFov, Type InNear, Type InFar)
			{
				Matrix4x4 newMatrix;
				newMatrix.elements[0][0] = (1.0f / InAspect) * (1.0f / (tan(InFov / 2.0f)));
				newMatrix.elements[0][1] = 0.0f;
				newMatrix.elements[0][2] = 0.0f;
				newMatrix.elements[0][3] = 0.0f;
				
				newMatrix.elements[1][0] = 0.0f;
				newMatrix.elements[1][1] = -(1.0f / tan(InFov / 2.0f));
				newMatrix.elements[1][2] = 0.0f;
				newMatrix.elements[1][3] = 0.0f;
				
				newMatrix.elements[2][0] = 0.0f;
				newMatrix.elements[2][1] = 0.0f;
				newMatrix.elements[2][2] = InFar / (InFar - InNear);
				newMatrix.elements[2][3] = 1.0f;
				
				newMatrix.elements[3][0] = 0.0f;
				newMatrix.elements[3][1] = 0.0f;
				newMatrix.elements[3][2] = (-(InNear * InFar)) / (InFar - InNear);
				newMatrix.elements[3][3] = 0.0f;
				return newMatrix;
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

			static Matrix4x4 CreateRotationAroundY(Type InAngleRad)
			{
				Matrix4x4 newMatrix;
				Type cosAngle = cos(InAngleRad);
				Type sinAngle = sin(InAngleRad);

				newMatrix.elements[0][0] = cosAngle;
				newMatrix.elements[0][1] = 0.0f;
				newMatrix.elements[0][2] = -sinAngle;
				newMatrix.elements[0][3] = 0.0f;

				newMatrix.elements[1][0] = 0.0f;
				newMatrix.elements[1][1] = 1.0;
				newMatrix.elements[1][2] = 0.0;
				newMatrix.elements[1][3] = 0.0f;

				newMatrix.elements[2][0] = sinAngle;
				newMatrix.elements[2][1] = 0.0;
				newMatrix.elements[2][2] = cosAngle;
				newMatrix.elements[2][3] = 0.0f;

				newMatrix.elements[3][0] = 0.0f;
				newMatrix.elements[3][1] = 0.0f;
				newMatrix.elements[3][2] = 0.0f;
				newMatrix.elements[3][3] = 1.0f;

				return newMatrix;
			}

			static Matrix4x4 CreateRotationAroundZ(Type InAngleRad)
			{
				Matrix4x4 new_matrix;
				Type cos_angle = cos(InAngleRad);
				Type sin_angle = sin(InAngleRad);

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

			static Matrix4x4 Transpose(const Matrix4x4& InMatrix)
			{
				Matrix4x4 newMatrix;
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						newMatrix.elements[row][column] = InMatrix.elements[column][row];
					}
				}
				return newMatrix;
			}

			static Matrix4x4 GetFastInverse(const Matrix4x4& InMatrix)
			{
				Matrix4x4 newMatrix;
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						newMatrix.elements[row][column] = InMatrix.elements[column][row];
					}
				}

				for (unsigned int column = 0; column < 3; ++column)
				{
					Type total = 0.0f;
					total += -InMatrix.elements[3][0] * newMatrix.elements[0][column];
					total += -InMatrix.elements[3][1] * newMatrix.elements[1][column];
					total += -InMatrix.elements[3][2] * newMatrix.elements[2][column];
					newMatrix.elements[3][column] = total;
				}

				newMatrix.elements[3][3] = 1.0f;
				return newMatrix;
			}

			static Matrix4x4 GetInverse(const Matrix4x4& InMatrix)
			{
				const Type* const m = InMatrix.data;
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

				det = static_cast<Type>(1.0 / static_cast<double>(det));

				for(size_t i = 0; i < 16; i++)
				{
					inv[i] *= det;
				}
				
				return invMat;
			}

			Matrix4x4 operator*(const Matrix4x4& InMatrix) const
			{
				Matrix4x4 newMatrix;
				for (unsigned int row = 0; row < 4; ++row)
				{
					for (unsigned int column = 0; column < 4; ++column)
					{
						Type total = 0.0f;
						for (int index = 0; index < 4; ++index)
						{
							total += elements[row][index] * InMatrix.elements[index][column];
						}
	
						newMatrix.elements[row][column] = total;
					}
				}
				return newMatrix;
			}
		};

		template<typename Type, int RowOffset, int ColumnOffset>
		Vector4<Type> operator * (const Vector4<Type>& InVector, const Matrix4x4<Type, RowOffset, ColumnOffset>& InMatrix)
		{
			Vector4<Type> newVector;
			for (unsigned int column = 0; column < 4; ++column)
			{
				Type total = 0.0f;
				total += InVector.x * InMatrix.elements[0][column];
				total += InVector.y * InMatrix.elements[1][column];
				total += InVector.z * InMatrix.elements[2][column];
				total += InVector.w * InMatrix.elements[3][column];
				newVector[column] = total;
			}

			return newVector;
		}

		template<typename Type, int RowOffset, int ColumnOffset>
		Vector4<Type> operator * (const Matrix4x4<Type, RowOffset, ColumnOffset>& InMatrix, const Vector4<Type>& InVector)
		{
			Vector4<Type> newVector;
			for (unsigned int column = 0; column < 4; ++column)
			{
				Type total = 0.0f;
				total += InMatrix.elements[0][column] * InVector.x;
				total += InMatrix.elements[1][column] * InVector.y;
				total += InMatrix.elements[2][column] * InVector.z;
				total += InMatrix.elements[3][column] * InVector.w;
				newVector[column] = total;
			}

			return newVector;
		}

	}
}
