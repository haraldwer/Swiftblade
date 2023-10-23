#pragma once

#include <functional>

#include "Swizzle.hpp"
#include "Utility/Math/DefaultInitialization.h"
#include "Utility/Math/SqareRoot.h"

#define ALIB_MATH_VECTOR2

namespace Utility
{
	namespace Math
	{
		template<typename Type, int alignment>
		struct alignas(alignment) Vector2
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
						union
						{
							Type y;
							Type height;
							Type green;
							Type g;
						};

						Vector2Swizzle<Type, -1, -1, alignment> xx;
						Vector2Swizzle<Type, -1, 0, alignment> xy;
						Vector2Swizzle<Type, 0, -1, alignment> yx;
						Vector2Swizzle<Type, 0, 0, alignment> yy;
						Vector3Swizzle<Type, -1, -1, -1, alignment> xxx;
						Vector3Swizzle<Type, -1, -1, 0, alignment> xxy;
						Vector3Swizzle<Type, -1, 0, -1, alignment> xyx;
						Vector3Swizzle<Type, -1, 0, 0, alignment> xyy;
						Vector3Swizzle<Type, 0, -1, -1, alignment> yxx;
						Vector3Swizzle<Type, 0, -1, 0, alignment> yxy;
						Vector3Swizzle<Type, 0, 0, -1, alignment> yyx;
						Vector3Swizzle<Type, 0, 0, 0, alignment> yyy;
						Vector4Swizzle<Type, -1, -1, -1, -1, alignment> xxxx;
						Vector4Swizzle<Type, -1, -1, -1, 0, alignment> xxxy;
						Vector4Swizzle<Type, -1, -1, 0, -1, alignment> xxyx;
						Vector4Swizzle<Type, -1, -1, 0, 0, alignment> xxyy;
						Vector4Swizzle<Type, -1, 0, -1, -1, alignment> xyxx;
						Vector4Swizzle<Type, -1, 0, -1, 0, alignment> xyxy;
						Vector4Swizzle<Type, -1, 0, 0, -1, alignment> xyyx;
						Vector4Swizzle<Type, -1, 0, 0, 0, alignment> xyyy;
						Vector4Swizzle<Type, 0, -1, -1, -1, alignment> yxxx;
						Vector4Swizzle<Type, 0, -1, -1, 0, alignment> yxxy;
						Vector4Swizzle<Type, 0, -1, 0, -1, alignment> yxyx;
						Vector4Swizzle<Type, 0, -1, 0, 0, alignment> yxyy;
						Vector4Swizzle<Type, 0, 0, -1, -1, alignment> yyxx;
						Vector4Swizzle<Type, 0, 0, -1, 0, alignment> yyxy;
						Vector4Swizzle<Type, 0, 0, 0, -1, alignment> yyyx;
						Vector4Swizzle<Type, 0, 0, 0, 0, alignment> yyyy;

						struct
						{
							operator Type() const
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								return vector.x * vector.x + vector.y * vector.y;
							}

							Type operator()() const
							{
								Type result = (*this);
								return result;
							}

							Vector2& operator = (Type aDesiredLengthSquared)
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								Type currentLength = vector.Length();
								vector *= (SquareRoot(aDesiredLengthSquared) / currentLength);
								return vector;
							}

						}LengthSqr, LengthSquared, MagnitudeSquared, lengthSquared, magnitudeSquared;


						struct
						{
							operator Type() const
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								return vector.x * vector.x + vector.y * vector.y;
							}

							Type operator()(Vector2 second_vector) const
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								return vector.x * second_vector.x + vector.y * second_vector.y;
							}

							Type operator()() const
							{
								Type result = (*this);
								return result;
							}

							Vector2& operator = (Type aDesiredDotProduct)
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								Type currentLength = vector.Length();
								vector *= (SquareRoot(aDesiredDotProduct) / currentLength);
								return vector;
							}

						}Dot, DotProduct;


						struct
						{
							void operator()(const Vector2& aFirstVector, const Vector2& aSecondVector, float aInterpolation) const
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								vector = ((aSecondVector - aFirstVector) * aInterpolation) + aFirstVector;
							}

							Vector2 operator()(const Vector2& aSecondVector, float aInterpolation) const
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								return ((aSecondVector - vector) * aInterpolation) + vector;
							}

							Vector2 operator()(float aInterpolation) const
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								return vector * aInterpolation;
							}

						}Lerp;

						struct
						{
							operator Type() const
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));

								return SquareRoot(vector.x * vector.x + vector.y * vector.y);
							}

							Type operator()() const
							{
								Type result = (*this);
								return result;
							}

							Vector2& operator = (Type aDesiredLength)
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								Type currentLength = (*this);
								vector *= (aDesiredLength / currentLength);
								return vector;
							}

						}Length, Magnitude, length, magnitude;

						struct
						{
							operator Vector2() const
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								Type length = vector.length;
								if (length > 0.0f)
									return Vector2(vector.x / length, vector.y / length);
								else
									return vector;
							}

							Vector2 operator()() const
							{
								Vector2 result = (*this);
								return result;
							}

						}GetNormalized, unit, Unit, Normalized, normalized;

						struct
						{
							operator Vector2&()
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								Type length = vector.length;
								if (length > 0.0f) // Will be removed in future versions.
								{
									vector.x /= length;
									vector.y /= length;
								}
								return vector;
							}

							Vector2& operator()()
							{
								Vector2& result = (*this);
								return result;
							}

						}Normalize, normalize;

						struct
						{
							operator Type() const
							{
								return 0.0f;
							}

							float operator()(Vector2 aSecondVector) const
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								return vector.x * aSecondVector.y - vector.y * aSecondVector.x;
							}

							float operator()() const
							{
								return 0.0f;
							}

						}Cross, cross, CrossProduct;

						struct
						{
							Vector2 operator()(const Vector2& aOntoVector) const
							{
								Vector2& vector = Memory::Cast<Vector2>(this, (-((int)sizeof(Type))));
								return (aOntoVector * (vector.Dot(aOntoVector) / aOntoVector.Dot()));
							}

						}Project;
					};
				};
			};

			Type& operator [](int aIndex)
			{
				return data[aIndex];
			}

			void Iterate(std::function<void(Type)> aElementFunction)
			{
				for (int index = 0; index < 2; ++index)
				{
					aElementFunction(data[index]);
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
				}
			}
			 
			Vector2& operator = (const Vector2 aAssignment)
			{
				x = aAssignment.x;
				y = aAssignment.y;
				return (*this);
			}

			template<typename SecondaryType, int Offset1, int Offset2>
			Vector2& operator = (const Vector2Swizzle<SecondaryType, Offset1, Offset2> &aAssignment)
			{
				x = aAssignment.x;
				y = aAssignment.y;
				return (*this);
			}
			
			Vector2& operator = (const Type aAssignment)
			{
				x = aAssignment;
				y = aAssignment;
				return (*this);
			}

			Vector2& operator = (const float* aAssignment)
			{
				x = aAssignment[0];
				y = aAssignment[1];
				return (*this);
			}

			operator Vector2Swizzle<Type, 0, 1> &()
			{
				return xy;
			}

			template<typename SecondaryType, int Offset1, int Offset2>
			Vector2(const Vector2Swizzle<SecondaryType, Offset1, Offset2>& aAssignment) : data{ aAssignment.x, aAssignment.y } {}
			Vector2(const Type& a_x, const Type& a_y) : data{ a_x, a_y } {}
			Vector2(const Type& a_v) : data{ a_v, a_v } {}
			Vector2() : data{ DefaultInitializationValue<Type>(), DefaultInitializationValue<Type>() } {}

			inline static constexpr Vector2<Type> Up { 0, 1 };
			inline static constexpr Vector2<Type> Forward { 0, 1 };
			inline static constexpr Vector2<Type> Right { 1, 0 };
			inline static constexpr Vector2<Type> Zero { 0, 0 };
			inline static constexpr Vector2<Type> One { 1, 1 };
		};
	}
}

#include "Operations.hpp"
