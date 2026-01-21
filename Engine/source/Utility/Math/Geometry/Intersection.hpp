
#pragma once

#include "../Vector/Vector.hpp"
#include "Utility/Math/SqareRoot.h"

namespace Utility
{
	namespace Math
	{
		template <class T>
		bool LinePlaneIntersection(Vector3<T>& InContact, const Vector3<T>& InRayDirection, const Vector3<T>& InRayOrigin, 
						   const Vector3<T>& InNormal, const Vector3<T>& InCoord) {
			// get d value
			
			T rayDot = InNormal.Dot(InRayDirection); 
			
			if (rayDot == 0) {
				return false; // No intersection, the line is parallel to the plane
			}

			T coordDot = InNormal.Dot(InCoord);
			T originDot = InNormal.Dot(InRayOrigin);
			
			// Compute the X value for the directed line ray intersecting the plane
			T x = (coordDot - originDot) / rayDot;

			// output contact point
			InContact = InRayOrigin + InRayDirection.GetNormalized() * x; //Make sure your ray vector is normalized
			return true;
		}

		/*inline bool CheckLineCollision(float p1s_x, float p1s_y, float p1e_x, float p1e_y,
		                               float p2s_x, float p2s_y, float p2e_x, float p2e_y,
		                               float &point_x, float &point_y)*/
		template<typename Type>
		bool FindIntersection_StaticLineVsStaticLine(
			const Vector2<Type> InFromA, const Vector2<Type> InToA,
			const Vector2<Type> InFromB, const Vector2<Type> InToB,
			Vector2<Type>& OutResult)
		{
		    const float ax = InToA.x - InFromA.x;
		    const float ay = InToA.y - InFromA.y;

		    const float bx = InToB.x - InFromB.x;
		    const float by = InToB.y - InFromB.y;
							   
		    const float dx = InFromA.x - InFromB.x;
		    const float dy = InFromA.y - InFromB.y;

		    const float u = -bx*ay+ax*by;

		    const float s = (-ay*dx+ax*dy)/u;
		    const float t = (bx*dy-by*dx)/u;

		    if (s >= 0.0 && s <= 1.0 && t >= 0.0 && t <= 1.0)
		    {
				OutResult.x = InFromA.x+ax*t;
				OutResult.y = InFromA.y+ay*t;
		        return true;
		    }

		    return false;
		}


		template<typename Type>
		bool FindIntersection_StaticLineVsStaticPlane(const Vector3<Type> InFrom, const Vector3<Type> InTo, const Vector3<Type> InPlanePosition, const Vector3<Type> InPlaneNormal, Vector3<Type>& OutResult)
		{
			float diff1[] = { InPlanePosition[0] - InFrom[0], InPlanePosition[1] - InFrom[1], InPlanePosition[2] - InFrom[2] };
			float diff2[] = { InTo[0] - InFrom[0], InTo[1] - InFrom[1], InTo[2] - InFrom[2] };

			const float r1 = InPlaneNormal[0] * diff1[0] + InPlaneNormal[1] * diff1[1] + InPlaneNormal[2] * diff1[2];
			const float r2 = InPlaneNormal[0] * diff2[0] + InPlaneNormal[1] * diff2[1] + InPlaneNormal[2] * diff2[2];

			if (r2 != 0.0)
			{
				const float factor = r1 / r2;

				if (factor >= 0.0 && factor <= 1.0)
				{
					float diff[] = { InTo[0] - InFrom[0], InTo[1] - InFrom[1], InTo[2] - InFrom[2] };

					OutResult[0] = InFrom[0] + diff[0] * factor;
					OutResult[1] = InFrom[1] + diff[1] * factor;
					OutResult[2] = InFrom[2] + diff[2] * factor;
					return true;
				}
			}

			return false;
		}

		template<typename Type>
		bool IsIntersecting_StaticCircleVsStaticCircle(const Vector2<Type> InCircleAPosition, const float InARadius, const Vector2<Type> InCircleBPosition, const float InBRadius)
		{
			const float total_radius = InARadius + InBRadius;
			if ((InCircleBPosition - InCircleAPosition).LengthSquared() <= total_radius * total_radius)
			{
				return true;
			}

			return false;
		}

		template<typename Type>
		bool IsIntersecting_StaticPointVsStaticAABB(const Vector2<Type> InPoint, const Vector4<Type> InAABB)
		{
			if (InPoint.x > InAABB.x && InPoint.x < InAABB.z
				&& InPoint.y > InAABB.y && InPoint.y < InAABB.w)
			{
				return true;
			}

			return false;
		}

		// Liang Barsky Clipper
		template<typename Type>
		bool FindClosestIntersectionFactor_MovingPointVsAABB(const Vector2<Type> InFrom, const Vector2<Type> InTo, const Vector4<Type> InAABB, Type& OutFactor)
		{
			if (IsIntersecting_StaticPointVsStaticAABB(InFrom, InAABB))
			{
				OutFactor = 0.0f;
				return true;
			}

			Vector4<Type> p = { -(InTo.x - InFrom.x), (InTo.x - InFrom.x), -(InTo.y - InFrom.y), (InTo.y - InFrom.y) };
			Vector4<Type> q = { InFrom.x - InAABB.x, InAABB.z - InFrom.x, InFrom.y - InAABB.y, InAABB.w - InFrom.y };

			Type factor1 = -std::numeric_limits<float>::infinity();
			Type factor2 = std::numeric_limits<float>::infinity();

			for (int index = 0; index < 4; ++index)
			{
				if (p[index] == 0.0f)
				{
					if (q[index] < 0.0f)
					{
						return false;
					}
				}
				else
				{
					Type t = q[index] / p[index];
					if (p[index] < 0.0f && factor1 < t)
					{
						factor1 = t;
					}
					else if (p[index] > 0.0f && factor2 > t)
					{
						factor2 = t;
					}
				}
			}

			if (factor1 > factor2 || factor1 > 1.0f || factor1 < 0.0f)
			{
				return false;
			}

			OutFactor = factor1;
			return true;
		}


		template<typename Type>
		bool FindClosestIntersectionFactor_MovingPointVsStaticCircle(const Vector2<Type> InFrom, const Vector2<Type> InTo, const Vector2<Type> InPos, const Type InRadius, Type& InFactor)
		{
			float initialDistance = (InFrom - InPos).LengthSquared();

			if (initialDistance < InRadius * InRadius)
			{
				InFactor = 0.0f;
				return true;
			}

			Vector2<Type> direction = InTo - InFrom;
			Vector2<Type> sphereDifference = InFrom - InPos;

			const float a = direction.Dot(direction);
			const float b = 2.0f * sphereDifference.Dot(direction);
			const float c = sphereDifference.Dot(sphereDifference) - (InRadius * InRadius);

			float discriminant = (b * b) - (4.0f * a * c);
			if (discriminant < 0)
			{

			}
			else
			{
				discriminant = SquareRoot(discriminant);

				float factorA = (-b - discriminant) / (2.0f * a);
				float factorB = (-b + discriminant) / (2.0f * a);

				if (factorA >= 0 && factorA <= 1)
				{
					InFactor = factorA;
					return true;
				}

				if (factorB >= 0 && factorB <= 1)
				{
					InFactor = factorB;
					return true;
				}

				return false;

			}
			return false;
		}

		template<typename Type>
		bool FindClosestIntersectionFactor_MovingCircleVsMovingCircle(const Vector2<Type> InCircleABegin, const Vector2<Type> InCircleAEnd, const Type InCircleARadius, const Vector2<Type> InCircleBBegin, const Vector2<Type> InCircleBEnd, const Type InCircleBRadius, Type& OutFactor)
		{
			auto total_radius = InCircleARadius + InCircleBRadius;
			float initialDistance = (InCircleBBegin - InCircleABegin).LengthSquared();

			if (initialDistance < total_radius * total_radius)
			{
				OutFactor = 0.0f;
				return true;
			}

			auto difference_a = InCircleAEnd - InCircleABegin;
			auto difference_b = InCircleBEnd - InCircleBBegin;



			auto circle_a_relative_end = InCircleAEnd - difference_b;

			return FindClosestIntersectionFactor_MovingPointVsStaticCircle(InCircleABegin, circle_a_relative_end, InCircleBBegin, total_radius, OutFactor);
		}

		template<typename Type>
		bool FindClosestIntersectionFactor_MovingCircleVsAABB(const Vector2<Type> InFrom, const Vector2<Type> InTo, const Type InRadius, const Vector4<Type> InAABB, Type& InFactor)
		{
			float smallestFactorFound = 10000.0f;

			float factorCandidate = smallestFactorFound;
			FindClosestIntersectionFactor_MovingPointVsAABB(InFrom, InTo, InAABB + Vector4<Type>(-InRadius, 0.0f, InRadius, 0.0f), factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			FindClosestIntersectionFactor_MovingPointVsAABB(InFrom, InTo, InAABB + Vector4<Type>(0.0f, -InRadius, 0.0f, InRadius), factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			FindClosestIntersectionFactor_MovingPointVsStaticCircle(InFrom, InTo, Vector2<Type>(InAABB.xy), InRadius, factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			FindClosestIntersectionFactor_MovingPointVsStaticCircle(InFrom, InTo, Vector2<Type>(InAABB.xw), InRadius, factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			FindClosestIntersectionFactor_MovingPointVsStaticCircle(InFrom, InTo, Vector2<Type>(InAABB.zy), InRadius, factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			FindClosestIntersectionFactor_MovingPointVsStaticCircle(InFrom, InTo, Vector2<Type>(InAABB.zw), InRadius, factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			InFactor = smallestFactorFound;
			return smallestFactorFound < 9999.0f;
		}


		template<typename Type>
		bool FindClosestIntersectedAtAndNormal_MovingCircleVsMovingCircle(const Vector2<Type> InCircleABegin, const Vector2<Type> InCircleAEnd, const Type InCircleARadius, const Vector2<Type> InCircleBBegin, const Vector2<Type> InCircleBEnd, const Type InCircleBRadius, Vector2<float>& InIntersection, Vector2<float>& InNormal)
		{
			float smallestFactorFound = 10000.0f;
			FindClosestIntersectionFactor_MovingCircleVsMovingCircle(InCircleABegin, InCircleAEnd, InCircleARadius, InCircleBBegin, InCircleBEnd, InCircleBRadius, smallestFactorFound);

			if (smallestFactorFound < 9999.0f)
			{
				auto p1 = InCircleABegin + (InCircleAEnd - InCircleABegin) * smallestFactorFound;
				auto p2 = InCircleBBegin + (InCircleBEnd - InCircleBBegin) * smallestFactorFound;

				InNormal = (p1 - p2).Normalized();

				InIntersection = p1;
			}

			return smallestFactorFound < 9999.0f;
		}


		template<typename Type>
		bool FindClosestIntersectedAtAndNormal_MovingCircleVsAABB(const Vector2<Type> InFrom, const Vector2<Type> InTo, const Type InRadius, const Vector4<Type> InAABB, Vector2<float>& InIntersection, Vector2<float>& InNormal)
		{
			float smallestFactorFound = 10000.0f;

			FindClosestIntersectionFactor_MovingCircleVsAABB(InFrom, InTo, InRadius, InAABB, smallestFactorFound);

			
			if (smallestFactorFound < 9999.0f)
			{
				InIntersection = InFrom + (InTo - InFrom) * smallestFactorFound;
				if (InIntersection.x < InAABB.x)
				{
					if (InIntersection.y < InAABB.y)
					{
						InNormal = (InIntersection - InAABB.xy).Normalized();
					}
					else if (InIntersection.y > InAABB.w)
					{
						InNormal = (InIntersection - InAABB.xw).Normalized();
					}
					else
					{
						InNormal = Vector2<Type>(-1.0f, 0.0f);
					}
				}
				else if (InIntersection.x > InAABB.z)
				{
					if (InIntersection.y < InAABB.y)
					{
						InNormal = (InIntersection - InAABB.zy).Normalized();
					}
					else if (InIntersection.y > InAABB.w)
					{
						InNormal = (InIntersection - InAABB.zw).Normalized();
					}
					else
					{
						InNormal = Vector2<Type>(1.0f, 0.0f);
					}
				}
				else
				{
					if (InIntersection.y < InAABB.y)
					{
						InNormal = Vector2<Type>(0.0f, -1.0f);
					}
					else if (InIntersection.y > InAABB.w)
					{
						InNormal = Vector2<Type>(0.0f, 1.0f);
					}
					else
					{
						InNormal = Vector2<Type>(0.0f, 0.0f);
					}
				}

			}

			return smallestFactorFound < 9999.0f;
		}
	}



}

