
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

			Type& operator()(const int aRow, const int aColumn)
			{
				return elements[(aRow - RowOffset)][(aColumn - ColumnOffset)];
			}

			const Type& operator()(const int aRow, const int aColumn) const
			{
				return elements[(aRow - RowOffset)][(aColumn - ColumnOffset)];
			}

			Matrix3x3 operator + (const Matrix3x3& aMatrix) const
			{
				Matrix3x3 new_matrix;
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						new_matrix.elements[row][column] = elements[row][column] + aMatrix.elements[row][column];
					}
				}
				return new_matrix;
			}

			Matrix3x3& operator += (const Matrix3x3& aMatrix)
			{
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						elements[row][column] += aMatrix.elements[row][column];
					}
				}
				return *this;
			}


			Matrix3x3 operator - (const Matrix3x3& aMatrix) const
			{
				Matrix3x3 new_matrix;
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						new_matrix.elements[row][column] = elements[row][column] - aMatrix.elements[row][column];
					}
				}
				return new_matrix;
			}

			Matrix3x3& operator -= (const Matrix3x3& aMatrix)
			{
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						elements[row][column] -= aMatrix.elements[row][column];
					}
				}
				return *this;
			}


			Matrix3x3 operator * (const Matrix3x3& aMatrix) const
			{
				Matrix3x3 new_matrix;
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						Type total = 0.0f;
						for (int index = 0; index < 3; ++index)
						{
							total += elements[row][index] * aMatrix.elements[index][column];
						}

						new_matrix.elements[row][column] = total;
					}
				}
				return new_matrix;
			}

			Matrix3x3& operator *= (const Matrix3x3& aMatrix)
			{
				return ((*this) = (*this) * aMatrix);
			}

			Matrix3x3& operator = (const Matrix3x3& aMatrix)
			{
				for (int index = 0; index < 9; ++index)
				{
					data[index] = aMatrix.data[index];
				}

				return *this;
			}

			bool operator == (const Matrix3x3& aMatrix) const
			{
				for (int index = 0; index < 9; ++index)
				{
					if (data[index] != aMatrix.data[index])
					{
						return false;
					}
				}

				return true;
			}

			// Static functions for creating rotation matrices.
			static Matrix3x3 CreateRotationAroundX(Type aAngleInRadians)
			{
				Matrix3x3 new_matrix;
				Type cos_angle = std::cos(aAngleInRadians);
				Type sin_angle = std::sin(aAngleInRadians);

				new_matrix.elements[0][0] = 1.0f;
				new_matrix.elements[0][1] = 0.0f;
				new_matrix.elements[0][2] = 0.0f;

				new_matrix.elements[1][0] = 0.0f;
				new_matrix.elements[1][1] = cos_angle;
				new_matrix.elements[1][2] = sin_angle;

				new_matrix.elements[2][0] = 0.0f;
				new_matrix.elements[2][1] = -sin_angle;
				new_matrix.elements[2][2] = cos_angle;
				return new_matrix;
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

			static Matrix3x3 CreateRotationAroundZ(Type aAngleInRadians)
			{
				Matrix3x3 new_matrix;
				Type cos_angle = std::cos(aAngleInRadians);
				Type sin_angle = std::sin(aAngleInRadians);

				new_matrix.elements[0][0] = cos_angle;
				new_matrix.elements[0][1] = sin_angle;
				new_matrix.elements[0][2] = 0.0f;
				
				new_matrix.elements[1][0] = -sin_angle;
				new_matrix.elements[1][1] = cos_angle;
				new_matrix.elements[1][2] = 0.0f;
				
				new_matrix.elements[2][0] = 0.0f;
				new_matrix.elements[2][1] = 0.0f;
				new_matrix.elements[2][2] = 1.0f;
				return new_matrix;
			}

			// Static function for creating a transpose of a matrix.
			static Matrix3x3 Transpose(const Matrix3x3& aMatrixToTranspose)
			{
				Matrix3x3 new_matrix;
				for (unsigned int row = 0; row < 3; ++row)
				{
					for (unsigned int column = 0; column < 3; ++column)
					{
						new_matrix.elements[row][column] = aMatrixToTranspose.elements[column][row];
					}
				}
				return new_matrix;
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

			Matrix3x3(const Matrix3x3& aMatrix)
			{
				elements[0][0] = aMatrix.elements[0][0];
				elements[0][1] = aMatrix.elements[0][1];
				elements[0][2] = aMatrix.elements[0][2];

				elements[1][0] = aMatrix.elements[1][0];
				elements[1][1] = aMatrix.elements[1][1];
				elements[1][2] = aMatrix.elements[1][2];

				elements[2][0] = aMatrix.elements[2][0];
				elements[2][1] = aMatrix.elements[2][1];
				elements[2][2] = aMatrix.elements[2][2];
			}
		};

		template<typename Type, int row_offset, int column_offset>
		Vector3<Type> operator * (Vector3<Type> aVector, Matrix3x3<Type, row_offset, column_offset> aMatrix)
		{
			Vector3<Type> new_vector;
			for (unsigned int column = 0; column < 3; ++column)
			{
				Type total = 0.0f;
				total += aVector.x * aMatrix.elements[0][column];
				total += aVector.y * aMatrix.elements[1][column];
				total += aVector.z * aMatrix.elements[2][column];
				new_vector[column] = total;
			}

			return new_vector;
		}

	}
}