
#pragma once

#include "../Vector/Vector.hpp"
#include "Utility/Math/SqareRoot.h"

namespace Utility
{
	namespace Math
	{
		template <class T>
		bool LinePlaneIntersection(Vector3<T>& contact, const Vector3<T>& rayDirection, const Vector3<T>& rayOrigin, 
						   const Vector3<T>& normal, const Vector3<T>& coord) {
			// get d value
			
			T rayDot = normal.Dot(rayDirection); 
			
			if (rayDot == 0) {
				return false; // No intersection, the line is parallel to the plane
			}

			T coordDot = normal.Dot(coord);
			T originDot = normal.Dot(rayOrigin);
			
			// Compute the X value for the directed line ray intersecting the plane
			T x = (coordDot - originDot) / rayDot;

			// output contact point
			contact = rayOrigin + rayDirection.GetNormalized() * x; //Make sure your ray vector is normalized
			return true;
		}

		/*inline bool CheckLineCollision(float p1s_x, float p1s_y, float p1e_x, float p1e_y,
		                               float p2s_x, float p2s_y, float p2e_x, float p2e_y,
		                               float &point_x, float &point_y)*/
		template<typename Type>
		inline bool FindIntersection_StaticLineVsStaticLine(const Vector2<Type> aFromA, const Vector2<Type> aToA,
															const Vector2<Type> aFromB, const Vector2<Type> aToB,
															Vector2<Type>& result)
		{
		    float ax = aToA.x - aFromA.x;
		    float ay = aToA.y - aFromA.y;

		    float bx = aToB.x - aFromB.x;
		    float by = aToB.y - aFromB.y;
							   
		    float dx = aFromA.x - aFromB.x;
		    float dy = aFromA.y - aFromB.y;

		    float u = -bx*ay+ax*by;

		    float s = (-ay*dx+ax*dy)/u;
		    float t = (bx*dy-by*dx)/u;

		    if (s >= 0.0 && s <= 1.0 && t >= 0.0 && t <= 1.0)
		    {
				result.x = aFromA.x+ax*t;
				result.y = aFromA.y+ay*t;
		        return true;
		    }

		    return false;
		}


		template<typename Type>
		inline bool FindIntersection_StaticLineVsStaticPlane(const Vector3<Type> from, const Vector3<Type> to, const Vector3<Type> plane_position, const Vector3<Type> plane_normal, Vector3<Type>& result)
		{
			float diff1[] = { plane_position[0] - from[0], plane_position[1] - from[1], plane_position[2] - from[2] };
			float diff2[] = { to[0] - from[0], to[1] - from[1], to[2] - from[2] };

			float r1 = plane_normal[0] * diff1[0] + plane_normal[1] * diff1[1] + plane_normal[2] * diff1[2];
			float r2 = plane_normal[0] * diff2[0] + plane_normal[1] * diff2[1] + plane_normal[2] * diff2[2];

			if (r2 != 0.0)
			{
				float factor = r1 / r2;

				if (factor >= 0.0 && factor <= 1.0)
				{
					float diff[] = { to[0] - from[0], to[1] - from[1], to[2] - from[2] };

					result[0] = from[0] + diff[0] * factor;
					result[1] = from[1] + diff[1] * factor;
					result[2] = from[2] + diff[2] * factor;
					return true;
				}
			}

			return false;
		}

		template<typename Type>
		inline bool IsIntersecting_StaticCircleVsStaticCircle(const Vector2<Type> circle_a_position, const float a_radius, const Vector2<Type> circle_b_position, const float b_radius)
		{
			const float total_radius = a_radius + b_radius;
			if ((circle_b_position - circle_a_position).LengthSquared() <= total_radius * total_radius)
			{
				return true;
			}

			return false;
		}

		template<typename Type>
		inline bool IsIntersecting_StaticPointVsStaticAABB(const Vector2<Type> point, const Vector4<Type> aabb)
		{
			if (point.x > aabb.x && point.x < aabb.z
				&& point.y > aabb.y && point.y < aabb.w)
			{
				return true;
			}

			return false;
		}

		// Liang Barsky Clipper
		template<typename Type>
		inline bool FindClosestIntersectionFactor_MovingPointVsAABB(const Vector2<Type> from, const Vector2<Type> to, const Vector4<Type> aabb, Type& factor)
		{
			if (IsIntersecting_StaticPointVsStaticAABB(from, aabb))
			{
				factor = 0.0f;
				return true;
			}

			Vector4<Type> p = { -(to.x - from.x), (to.x - from.x), -(to.y - from.y), (to.y - from.y) };
			Vector4<Type> q = { from.x - aabb.x, aabb.z - from.x, from.y - aabb.y, aabb.w - from.y };

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

			factor = factor1;
			return true;
		}


		template<typename Type>
		inline bool FindClosestIntersectionFactor_MovingPointVsStaticCircle(const Vector2<Type> from, const Vector2<Type> to, const Vector2<Type> position, const Type radius, Type& factor)
		{
			float initialDistance = (from - position).LengthSquared();

			if (initialDistance < radius * radius)
			{
				factor = 0.0f;
				return true;
			}

			Vector2<Type> direction = to - from;
			Vector2<Type> sphereDifference = from - position;

			float a = direction.Dot(direction);
			float b = 2.0f * sphereDifference.Dot(direction);
			float c = sphereDifference.Dot(sphereDifference) - (radius * radius);

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
					factor = factorA;
					return true;
				}

				if (factorB >= 0 && factorB <= 1)
				{
					factor = factorB;
					return true;
				}

				return false;

			}
			return false;
		}

		template<typename Type>
		inline bool FindClosestIntersectionFactor_MovingCircleVsMovingCircle(const Vector2<Type> circle_a_begin, const Vector2<Type> circle_a_end, const Type circle_a_radius, const Vector2<Type> circle_b_begin, const Vector2<Type> circle_b_end, const Type circle_b_radius, Type& factor)
		{
			auto total_radius = circle_a_radius + circle_b_radius;
			float initialDistance = (circle_b_begin - circle_a_begin).LengthSquared();

			if (initialDistance < total_radius * total_radius)
			{
				factor = 0.0f;
				return true;
			}

			auto difference_a = circle_a_end - circle_a_begin;
			auto difference_b = circle_b_end - circle_b_begin;



			auto circle_a_relative_end = circle_a_end - difference_b;

			return FindClosestIntersectionFactor_MovingPointVsStaticCircle(circle_a_begin, circle_a_relative_end, circle_b_begin, total_radius, factor);
		}

		template<typename Type>
		inline bool FindClosestIntersectionFactor_MovingCircleVsAABB(const Vector2<Type> from, const Vector2<Type> to, const Type radius, const Vector4<Type> aabb, Type& factor)
		{
			float smallestFactorFound = 10000.0f;

			float factorCandidate = smallestFactorFound;
			FindClosestIntersectionFactor_MovingPointVsAABB(from, to, aabb + Vector4<Type>(-radius, 0.0f, radius, 0.0f), factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			FindClosestIntersectionFactor_MovingPointVsAABB(from, to, aabb + Vector4<Type>(0.0f, -radius, 0.0f, radius), factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			FindClosestIntersectionFactor_MovingPointVsStaticCircle(from, to, Vector2<Type>(aabb.xy), radius, factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			FindClosestIntersectionFactor_MovingPointVsStaticCircle(from, to, Vector2<Type>(aabb.xw), radius, factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			FindClosestIntersectionFactor_MovingPointVsStaticCircle(from, to, Vector2<Type>(aabb.zy), radius, factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			FindClosestIntersectionFactor_MovingPointVsStaticCircle(from, to, Vector2<Type>(aabb.zw), radius, factorCandidate);
			smallestFactorFound = smallestFactorFound > factorCandidate ? factorCandidate : smallestFactorFound;

			factor = smallestFactorFound;
			return smallestFactorFound < 9999.0f;
		}


		template<typename Type>
		inline bool FindClosestIntersectedAtAndNormal_MovingCircleVsMovingCircle(const Vector2<Type> circle_a_begin, const Vector2<Type> circle_a_end, const Type circle_a_radius, const Vector2<Type> circle_b_begin, const Vector2<Type> circle_b_end, const Type circle_b_radius, Vector2<float>& intersection, Vector2<float>& normal)
		{
			float smallestFactorFound = 10000.0f;
			FindClosestIntersectionFactor_MovingCircleVsMovingCircle(circle_a_begin, circle_a_end, circle_a_radius, circle_b_begin, circle_b_end, circle_b_radius, smallestFactorFound);

			if (smallestFactorFound < 9999.0f)
			{
				auto p1 = circle_a_begin + (circle_a_end - circle_a_begin) * smallestFactorFound;
				auto p2 = circle_b_begin + (circle_b_end - circle_b_begin) * smallestFactorFound;

				normal = (p1 - p2).Normalized();

				intersection = p1;
			}

			return smallestFactorFound < 9999.0f;
		}


		template<typename Type>
		inline bool FindClosestIntersectedAtAndNormal_MovingCircleVsAABB(const Vector2<Type> from, const Vector2<Type> to, const Type radius, const Vector4<Type> aabb, Vector2<float>& intersection, Vector2<float>& normal)
		{
			float smallestFactorFound = 10000.0f;

			FindClosestIntersectionFactor_MovingCircleVsAABB(from, to, radius, aabb, smallestFactorFound);

			
			if (smallestFactorFound < 9999.0f)
			{
				intersection = from + (to - from) * smallestFactorFound;
				if (intersection.x < aabb.x)
				{
					if (intersection.y < aabb.y)
					{
						normal = (intersection - aabb.xy).Normalized();
					}
					else if (intersection.y > aabb.w)
					{
						normal = (intersection - aabb.xw).Normalized();
					}
					else
					{
						normal = Vector2<Type>(-1.0f, 0.0f);
					}
				}
				else if (intersection.x > aabb.z)
				{
					if (intersection.y < aabb.y)
					{
						normal = (intersection - aabb.zy).Normalized();
					}
					else if (intersection.y > aabb.w)
					{
						normal = (intersection - aabb.zw).Normalized();
					}
					else
					{
						normal = Vector2<Type>(1.0f, 0.0f);
					}
				}
				else
				{
					if (intersection.y < aabb.y)
					{
						normal = Vector2<Type>(0.0f, -1.0f);
					}
					else if (intersection.y > aabb.w)
					{
						normal = Vector2<Type>(0.0f, 1.0f);
					}
					else
					{
						normal = Vector2<Type>(0.0f, 0.0f);
					}
				}

			}

			return smallestFactorFound < 9999.0f;
		}
	}



}

