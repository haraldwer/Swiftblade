#pragma once

#include "Swizzle.hpp"
#include "Utility/Math/DefaultInitialization.h"

#define ALIB_MATH_VECTOR3

namespace Utility
{
	namespace Math
	{
		template<typename Type, int alignment>
		struct alignas(alignment) Vector3
		{
			union
			{
				Type data[3];

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

					union
					{

						union
						{
							Type z;
							Type depth;
							Type blue;
							Type b;
						};

						Vector2Swizzle<Type, -2, -2, alignment> xx;
						Vector2Swizzle<Type, -2, -1, alignment> xy;
						Vector2Swizzle<Type, -2, 0, alignment> xz;
						Vector2Swizzle<Type, -1, -2, alignment> yx;
						Vector2Swizzle<Type, -1, -1, alignment> yy;
						Vector2Swizzle<Type, -1, 0, alignment> yz;
						Vector2Swizzle<Type, 0, -2, alignment> zx;
						Vector2Swizzle<Type, 0, -1, alignment> zy;
						Vector2Swizzle<Type, 0, 0, alignment> zz;
						Vector3Swizzle<Type, -2, -2, -2, alignment> xxx;
						Vector3Swizzle<Type, -2, -2, -1, alignment> xxy;
						Vector3Swizzle<Type, -2, -2, 0, alignment> xxz;
						Vector3Swizzle<Type, -2, -1, -2, alignment> xyx;
						Vector3Swizzle<Type, -2, -1, -1, alignment> xyy;
						Vector3Swizzle<Type, -2, -1, 0, alignment> xyz;
						Vector3Swizzle<Type, -2, 0, -2, alignment> xzx;
						Vector3Swizzle<Type, -2, 0, -1, alignment> xzy;
						Vector3Swizzle<Type, -2, 0, 0, alignment> xzz;
						Vector3Swizzle<Type, -1, -2, -2, alignment> yxx;
						Vector3Swizzle<Type, -1, -2, -1, alignment> yxy;
						Vector3Swizzle<Type, -1, -2, 0, alignment> yxz;
						Vector3Swizzle<Type, -1, -1, -2, alignment> yyx;
						Vector3Swizzle<Type, -1, -1, -1, alignment> yyy;
						Vector3Swizzle<Type, -1, -1, 0, alignment> yyz;
						Vector3Swizzle<Type, -1, 0, -2, alignment> yzx;
						Vector3Swizzle<Type, -1, 0, -1, alignment> yzy;
						Vector3Swizzle<Type, -1, 0, 0, alignment> yzz;
						Vector3Swizzle<Type, 0, -2, -2, alignment> zxx;
						Vector3Swizzle<Type, 0, -2, -1, alignment> zxy;
						Vector3Swizzle<Type, 0, -2, 0, alignment> zxz;
						Vector3Swizzle<Type, 0, -1, -2, alignment> zyx;
						Vector3Swizzle<Type, 0, -1, -1, alignment> zyy;
						Vector3Swizzle<Type, 0, -1, 0, alignment> zyz;
						Vector3Swizzle<Type, 0, 0, -2, alignment> zzx;
						Vector3Swizzle<Type, 0, 0, -1, alignment> zzy;
						Vector3Swizzle<Type, 0, 0, 0, alignment> zzz;
						Vector4Swizzle<Type, -2, -2, -2, -2, alignment> xxxx;
						Vector4Swizzle<Type, -2, -2, -2, -1, alignment> xxxy;
						Vector4Swizzle<Type, -2, -2, -2, 0, alignment> xxxz;
						Vector4Swizzle<Type, -2, -2, -1, -2, alignment> xxyx;
						Vector4Swizzle<Type, -2, -2, -1, -1, alignment> xxyy;
						Vector4Swizzle<Type, -2, -2, -1, 0, alignment> xxyz;
						Vector4Swizzle<Type, -2, -2, 0, -2, alignment> xxzx;
						Vector4Swizzle<Type, -2, -2, 0, -1, alignment> xxzy;
						Vector4Swizzle<Type, -2, -2, 0, 0, alignment> xxzz;
						Vector4Swizzle<Type, -2, -1, -2, -2, alignment> xyxx;
						Vector4Swizzle<Type, -2, -1, -2, -1, alignment> xyxy;
						Vector4Swizzle<Type, -2, -1, -2, 0, alignment> xyxz;
						Vector4Swizzle<Type, -2, -1, -1, -2, alignment> xyyx;
						Vector4Swizzle<Type, -2, -1, -1, -1, alignment> xyyy;
						Vector4Swizzle<Type, -2, -1, -1, 0, alignment> xyyz;
						Vector4Swizzle<Type, -2, -1, 0, -2, alignment> xyzx;
						Vector4Swizzle<Type, -2, -1, 0, -1, alignment> xyzy;
						Vector4Swizzle<Type, -2, -1, 0, 0, alignment> xyzz;
						Vector4Swizzle<Type, -2, 0, -2, -2, alignment> xzxx;
						Vector4Swizzle<Type, -2, 0, -2, -1, alignment> xzxy;
						Vector4Swizzle<Type, -2, 0, -2, 0, alignment> xzxz;
						Vector4Swizzle<Type, -2, 0, -1, -2, alignment> xzyx;
						Vector4Swizzle<Type, -2, 0, -1, -1, alignment> xzyy;
						Vector4Swizzle<Type, -2, 0, -1, 0, alignment> xzyz;
						Vector4Swizzle<Type, -2, 0, 0, -2, alignment> xzzx;
						Vector4Swizzle<Type, -2, 0, 0, -1, alignment> xzzy;
						Vector4Swizzle<Type, -2, 0, 0, 0, alignment> xzzz;
						Vector4Swizzle<Type, -1, -2, -2, -2, alignment> yxxx;
						Vector4Swizzle<Type, -1, -2, -2, -1, alignment> yxxy;
						Vector4Swizzle<Type, -1, -2, -2, 0, alignment> yxxz;
						Vector4Swizzle<Type, -1, -2, -1, -2, alignment> yxyx;
						Vector4Swizzle<Type, -1, -2, -1, -1, alignment> yxyy;
						Vector4Swizzle<Type, -1, -2, -1, 0, alignment> yxyz;
						Vector4Swizzle<Type, -1, -2, 0, -2, alignment> yxzx;
						Vector4Swizzle<Type, -1, -2, 0, -1, alignment> yxzy;
						Vector4Swizzle<Type, -1, -2, 0, 0, alignment> yxzz;
						Vector4Swizzle<Type, -1, -1, -2, -2, alignment> yyxx;
						Vector4Swizzle<Type, -1, -1, -2, -1, alignment> yyxy;
						Vector4Swizzle<Type, -1, -1, -2, 0, alignment> yyxz;
						Vector4Swizzle<Type, -1, -1, -1, -2, alignment> yyyx;
						Vector4Swizzle<Type, -1, -1, -1, -1, alignment> yyyy;
						Vector4Swizzle<Type, -1, -1, -1, 0, alignment> yyyz;
						Vector4Swizzle<Type, -1, -1, 0, -2, alignment> yyzx;
						Vector4Swizzle<Type, -1, -1, 0, -1, alignment> yyzy;
						Vector4Swizzle<Type, -1, -1, 0, 0, alignment> yyzz;
						Vector4Swizzle<Type, -1, 0, -2, -2, alignment> yzxx;
						Vector4Swizzle<Type, -1, 0, -2, -1, alignment> yzxy;
						Vector4Swizzle<Type, -1, 0, -2, 0, alignment> yzxz;
						Vector4Swizzle<Type, -1, 0, -1, -2, alignment> yzyx;
						Vector4Swizzle<Type, -1, 0, -1, -1, alignment> yzyy;
						Vector4Swizzle<Type, -1, 0, -1, 0, alignment> yzyz;
						Vector4Swizzle<Type, -1, 0, 0, -2, alignment> yzzx;
						Vector4Swizzle<Type, -1, 0, 0, -1, alignment> yzzy;
						Vector4Swizzle<Type, -1, 0, 0, 0, alignment> yzzz;
						Vector4Swizzle<Type, 0, -2, -2, -2, alignment> zxxx;
						Vector4Swizzle<Type, 0, -2, -2, -1, alignment> zxxy;
						Vector4Swizzle<Type, 0, -2, -2, 0, alignment> zxxz;
						Vector4Swizzle<Type, 0, -2, -1, -2, alignment> zxyx;
						Vector4Swizzle<Type, 0, -2, -1, -1, alignment> zxyy;
						Vector4Swizzle<Type, 0, -2, -1, 0, alignment> zxyz;
						Vector4Swizzle<Type, 0, -2, 0, -2, alignment> zxzx;
						Vector4Swizzle<Type, 0, -2, 0, -1, alignment> zxzy;
						Vector4Swizzle<Type, 0, -2, 0, 0, alignment> zxzz;
						Vector4Swizzle<Type, 0, -1, -2, -2, alignment> zyxx;
						Vector4Swizzle<Type, 0, -1, -2, -1, alignment> zyxy;
						Vector4Swizzle<Type, 0, -1, -2, 0, alignment> zyxz;
						Vector4Swizzle<Type, 0, -1, -1, -2, alignment> zyyx;
						Vector4Swizzle<Type, 0, -1, -1, -1, alignment> zyyy;
						Vector4Swizzle<Type, 0, -1, -1, 0, alignment> zyyz;
						Vector4Swizzle<Type, 0, -1, 0, -2, alignment> zyzx;
						Vector4Swizzle<Type, 0, -1, 0, -1, alignment> zyzy;
						Vector4Swizzle<Type, 0, -1, 0, 0, alignment> zyzz;
						Vector4Swizzle<Type, 0, 0, -2, -2, alignment> zzxx;
						Vector4Swizzle<Type, 0, 0, -2, -1, alignment> zzxy;
						Vector4Swizzle<Type, 0, 0, -2, 0, alignment> zzxz;
						Vector4Swizzle<Type, 0, 0, -1, -2, alignment> zzyx;
						Vector4Swizzle<Type, 0, 0, -1, -1, alignment> zzyy;
						Vector4Swizzle<Type, 0, 0, -1, 0, alignment> zzyz;
						Vector4Swizzle<Type, 0, 0, 0, -2, alignment> zzzx;
						Vector4Swizzle<Type, 0, 0, 0, -1, alignment> zzzy;
						Vector4Swizzle<Type, 0, 0, 0, 0, alignment> zzzz;

						struct
						{
							operator Type() const
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
							}

							Type operator()() const
							{
								Type result = (*this);
								return result;
							}

							Vector3& operator = (Type aDesiredLengthSquared)
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								Type currentLength = vector.Length();
								vector *= (SquareRoot(aDesiredLengthSquared) / currentLength);
								return vector;
							}

						}LengthSqr, LengthSquared, MagnitudeSquared, lengthSquared, magnitudeSquared;

						struct
						{
							operator Type() const
							{
								const Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
							}

							Type operator()(Vector3 second_vector) const
							{
								const Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								return vector.x * second_vector.x + vector.y * second_vector.y + vector.z * second_vector.z;
							}

							Type operator()() const
							{
								Type result = (*this);
								return result;
							}

							Vector3& operator = (Type aDesiredDotProduct)
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								Type currentLength = vector.Length();
								vector *= (SquareRoot(aDesiredDotProduct) / currentLength);
								return vector;
							}

						}Dot, DotProduct;


						struct
						{
							Vector3 operator()(const Vector3& aFirstVector, const Vector3& aSecondVector, float aInterpolation) const
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								return ((aSecondVector - aFirstVector) * aInterpolation) + aFirstVector;
							}

							Vector3 operator()(const Vector3& aSecondVector, float aInterpolation) const
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								return ((aSecondVector - vector) * aInterpolation) + vector;
							}

							Vector3 operator()(float aInterpolation) const
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								return vector * aInterpolation;
							}

						}Lerp;

						struct
						{
							operator Type() const
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));

								return SquareRoot(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
							}

							Type operator()() const
							{
								Type result = (*this);
								return result;
							}

							Vector3& operator = (Type aDesiredLength)
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								Type currentLength = (*this);
								vector *= (aDesiredLength / currentLength);
								return vector;
							}

						}Length, Magnitude, length, magnitude;

				

						struct
						{
							operator Vector3() const
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								Type length = vector.length;
								if (length > 0.0f)
								{
									return Vector3(vector.x / length, vector.y / length, vector.z / length);
								}

								return vector;
							}

							Vector3 operator()() const
							{
								Vector3 result = (*this);
								return result;
							}

						}GetNormalized, unit, Unit, Normalized, normalized;

						struct
						{
							operator Vector3()
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								Type length = vector.length;
								if (length > 0.0)
								{
									vector.x /= length;
									vector.y /= length;
									vector.z /= length;
								}
								return vector;
							}

							Vector3 operator()()
							{
								Vector3 result = (*this);
								return result;
							}

						}Normalize, normalize;

						struct
						{
							Vector3 operator()(const Vector3& aOntoVector) const
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type))));
								return (aOntoVector * (vector.Dot(aOntoVector) / aOntoVector.Dot()));
							}

						}Project;

						struct
						{
							operator Vector3() const
							{
								return Vector3(0.0f, 0.0f, 0.0f);
							}

							Vector3 operator()(Vector3 aSecondVector) const
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));
								return Vector3(vector.y * aSecondVector.z - vector.z * aSecondVector.y, vector.z * aSecondVector.x - vector.x * aSecondVector.z, vector.x * aSecondVector.y - vector.y * aSecondVector.x);
							}

							Vector3 operator()() const
							{
								return Vector3(0.0f, 0.0f, 0.0f);
							}

						}Cross, cross, CrossProduct;



						struct
						{
							Vector3 operator()(Vector3 aReflectionNormal) const
							{
								Vector3& vector = Memory::Cast<Vector3>(this, (-((int)sizeof(Type) * 2)));

								return (vector - (2.0f * (vector.Dot(aReflectionNormal)))* aReflectionNormal);
							}

						}Reflect;






					};
				};
			};

			const Type& operator [](int index) const
			{
				return data[index];
			}

			Type& operator [](int index)
			{
				return data[index];
			}

			void SetMagnitude(Type aMagnitude)
			{
				float lengthSqr = LengthSqr;
				if (lengthSqr > 0)
				{
					Type ratio = aMagnitude / SquareRoot(lengthSqr);
					x *= ratio;
					y *= ratio;
					z *= ratio;
				}
			}

			void Limit(Type aMaxLimit)
			{
				float lengthSqr = LengthSqr;
				if (lengthSqr > aMaxLimit * aMaxLimit && lengthSqr > 0)
				{
					Type ratio = aMaxLimit / SquareRoot(lengthSqr);
					x *= ratio;
					y *= ratio;
					z *= ratio;
				}
			}

			void Iterate(std::function<void(Type)> aElementFunction)
			{
				for (int index = 0; index < 3; ++index)
				{
					aElementFunction(data[index]);
				}
			}

			Vector3& operator = (const Vector3& aAssignment)
			{
				x = aAssignment.x;
				y = aAssignment.y;
				z = aAssignment.z;
				return (*this);
			}

			template<typename SecondaryType, int Offset1, int Offset2, int Offset3>
			Vector3& operator = (const Vector3Swizzle<SecondaryType, Offset1, Offset2, Offset3>& aAssignment)
			{
				x = aAssignment.x;
				y = aAssignment.y;
				z = aAssignment.z;
				return (*this);
			}

			Vector3& operator = (const Type aAssignment)
			{
				x = aAssignment;
				y = aAssignment;
				z = aAssignment;
				return (*this);
			}

			Vector3& operator = (const float* aAssignment)
			{
				x = aAssignment[0];
				y = aAssignment[1];
				z = aAssignment[2];
				return (*this);
			}

			Vector3(const Type& a_x, const Type& a_y, const Type& a_z) : data{ a_x, a_y, a_z } {}
			Vector3(const Type& a_v) : data{ a_v, a_v, a_v } {}
			Vector3() : data{ DefaultInitializationValue<Type>(), DefaultInitializationValue<Type>(), DefaultInitializationValue<Type>() } {}

			static Vector3 Up() { return { 0, 1, 0 }; }
			static Vector3 Right() { return { 1, 0, 0 }; }
			static Vector3 Forward() { return { 0, 0, 1 }; }
			static Vector3 Zero() { return { 0, 0, 0 }; }
			static Vector3 One() { return { 1, 1, 1 }; }
		};
	}
}

#include "Operations.hpp"
