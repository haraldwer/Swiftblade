
#pragma once

#include "../Vector/Vector3.hpp"

namespace Utility
{
	namespace Math
	{
		template<typename Type, int RowOffset = 0, int ColumnOffset = 0>
		class Matrix3x3
		{
		public:
			union
			{
				Type elements[3][3];
				Type data[9];

				struct
				{
					Vector3<Type> right;
					Vector3<Type> up;
					Vector3<Type> forward;
				};
			};

			Type& operator()(const int InRow, const int InColumn)
			{
				return elements[(InRow - RowOffset)][(InColumn - ColumnOffset)];
			}

			const Type& operator()(const int InRow, const int InColumn) const
			{
				return elements[(InRow - RowOffset)][(InColumn - ColumnOffset)];
			}

			Matrix3x3 operator + (const Matrix3x3& InMatrix) const
			{
				Matrix3x3 newMatrix;
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						newMatrix.elements[row][column] = elements[row][column] + InMatrix.elements[row][column];
					}
				}
				return newMatrix;
			}

			Matrix3x3& operator += (const Matrix3x3& InMatrix)
			{
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						elements[row][column] += InMatrix.elements[row][column];
					}
				}
				return *this;
			}


			Matrix3x3 operator - (const Matrix3x3& InMatrix) const
			{
				Matrix3x3 newMatrix;
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						newMatrix.elements[row][column] = elements[row][column] - InMatrix.elements[row][column];
					}
				}
				return newMatrix;
			}

			Matrix3x3& operator -= (const Matrix3x3& InMatrix)
			{
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						elements[row][column] -= InMatrix.elements[row][column];
					}
				}
				return *this;
			}


			Matrix3x3 operator * (const Matrix3x3& InMatrix) const
			{
				Matrix3x3 newMatrix;
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						Type total = 0.0f;
						for (int index = 0; index < 3; ++index)
						{
							total += elements[row][index] * InMatrix.elements[index][column];
						}

						newMatrix.elements[row][column] = total;
					}
				}
				return newMatrix;
			}

			Matrix3x3& operator *= (const Matrix3x3& InMatrix)
			{
				return ((*this) = (*this) * InMatrix);
			}

			Matrix3x3& operator = (const Matrix3x3& InMatrix)
			{
				for (int index = 0; index < 9; ++index)
				{
					data[index] = InMatrix.data[index];
				}

				return *this;
			}

			bool operator == (const Matrix3x3& InMatrix) const
			{
				for (int index = 0; index < 9; ++index)
				{
					if (data[index] != InMatrix.data[index])
					{
						return false;
					}
				}

				return true;
			}

			// Static functions for creating rotation matrices.
			static Matrix3x3 CreateRotationAroundX(Type InAngleInRadians)
			{
				Matrix3x3 newMatrix;
				Type cosAngle = std::cos(InAngleInRadians);
				Type sinAngle = std::sin(InAngleInRadians);

				newMatrix.elements[0][0] = 1.0f;
				newMatrix.elements[0][1] = 0.0f;
				newMatrix.elements[0][2] = 0.0f;

				newMatrix.elements[1][0] = 0.0f;
				newMatrix.elements[1][1] = cosAngle;
				newMatrix.elements[1][2] = sinAngle;

				newMatrix.elements[2][0] = 0.0f;
				newMatrix.elements[2][1] = -sinAngle;
				newMatrix.elements[2][2] = cosAngle;
				return newMatrix;
			}

			static Matrix3x3 CreateRotationAroundY(Type aAngleInRadians)
			{
				Matrix3x3 new_matrix;
				Type cos_angle = std::cos(aAngleInRadians);
				Type sin_angle = std::sin(aAngleInRadians);

				new_matrix.elements[0][0] = cos_angle;
				new_matrix.elements[0][1] = 0.0f;
				new_matrix.elements[0][2] = -sin_angle;
				
				new_matrix.elements[1][0] = 0.0f;
				new_matrix.elements[1][1] = 1.0;
				new_matrix.elements[1][2] = 0.0;
				
				new_matrix.elements[2][0] = sin_angle;
				new_matrix.elements[2][1] = 0.0;
				new_matrix.elements[2][2] = cos_angle;
				return new_matrix;
			}

			static Matrix3x3 CreateRotationAroundZ(Type InAngleInRadians)
			{
				Matrix3x3 newMatrix;
				Type cosAngle = std::cos(InAngleInRadians);
				Type sinAngle = std::sin(InAngleInRadians);

				newMatrix.elements[0][0] = cosAngle;
				newMatrix.elements[0][1] = sinAngle;
				newMatrix.elements[0][2] = 0.0f;
				
				newMatrix.elements[1][0] = -sinAngle;
				newMatrix.elements[1][1] = cosAngle;
				newMatrix.elements[1][2] = 0.0f;
				
				newMatrix.elements[2][0] = 0.0f;
				newMatrix.elements[2][1] = 0.0f;
				newMatrix.elements[2][2] = 1.0f;
				return newMatrix;
			}

			// Static function for creating a transpose of a matrix.
			static Matrix3x3 Transpose(const Matrix3x3& InMatrixToTranspose)
			{
				Matrix3x3 newMatrix;
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						newMatrix.elements[row][column] = InMatrixToTranspose.elements[column][row];
					}
				}
				return newMatrix;
			}
			
			Matrix3x3() 
			{
				elements[0][0] = 1.0f;
				elements[0][1] = 0.0f;
				elements[0][2] = 0.0f;

				elements[1][0] = 0.0f;
				elements[1][1] = 1.0f;
				elements[1][2] = 0.0f;

				elements[2][0] = 0.0f;
				elements[2][1] = 0.0f;
				elements[2][2] = 1.0f;
			}

			Matrix3x3(const Matrix3x3& InMatrix)
			{
				elements[0][0] = InMatrix.elements[0][0];
				elements[0][1] = InMatrix.elements[0][1];
				elements[0][2] = InMatrix.elements[0][2];

				elements[1][0] = InMatrix.elements[1][0];
				elements[1][1] = InMatrix.elements[1][1];
				elements[1][2] = InMatrix.elements[1][2];

				elements[2][0] = InMatrix.elements[2][0];
				elements[2][1] = InMatrix.elements[2][1];
				elements[2][2] = InMatrix.elements[2][2];
			}
		};

		template<typename Type, int RowOffset, int ColumnOffset>
		Vector3<Type> operator * (Vector3<Type> InVector, Matrix3x3<Type, RowOffset, ColumnOffset> InMatrix)
		{
			Vector3<Type> newVector;
			for (unsigned int column = 0; column < 3; ++column)
			{
				Type total = 0.0f;
				total += InVector.x * InMatrix.elements[0][column];
				total += InVector.y * InMatrix.elements[1][column];
				total += InVector.z * InMatrix.elements[2][column];
				newVector[column] = total;
			}

			return newVector;
		}

	}
}