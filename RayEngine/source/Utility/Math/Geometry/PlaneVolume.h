#pragma once
#include <vector>

#include "Intersection.hpp"
#include "Plane.h"

namespace Utility
{
	namespace Math
	{
		template <class T>
		class PlaneVolume
		{
		public:
			PlaneVolume();
			PlaneVolume(const std::vector<Plane<T>>& InPlaneList);
			void AddPlane(const Plane<T>& InPlane);
			bool IsInside(const Vector3<T>& InPosition);
			bool FindIntersect(Vector3<T>& InResult, const Vector3<T>& InRayOrigin, const Vector3<T>& InRayDirection);
		private:
			std::vector<Plane<T>> myPlanes;
		};

		template <class T>
		PlaneVolume<T>::PlaneVolume()
		{
			myPlanes = std::vector<Plane<T>>();
		}
	
		template<class T>
		inline PlaneVolume<T>::PlaneVolume(const std::vector<Plane<T>>& InPlaneList)
		{
			myPlanes.clear();
			for (auto& it : InPlaneList)
			{
				myPlanes.push_back(it);
			}
		}
	
		template<class T>
		inline void PlaneVolume<T>::AddPlane(const Plane<T>& InPlane)
		{
			myPlanes.push_back(InPlane);
		}
	
		template<class T>
		inline bool PlaneVolume<T>::IsInside(const Vector3<T>& InPosition)
		{
			for(auto& it : myPlanes)
			{
				if(!it.IsInside(InPosition))
				{
					return false;
				}
			}
			return true;
		}

		template <class T>
		bool PlaneVolume<T>::FindIntersect(Vector3<T>& InResult, const Vector3<T>& InRayOrigin, const Vector3<T>& InRayDirection)
		{
			if (IsInside(InRayOrigin))
			{
				InResult = InRayOrigin; 
				return true; 
			}
			
			float distSqr = -1.0f;
			bool hit = false; 
			for (auto& plane : myPlanes)
			{
				Vector3<T> r; 
				if (!LinePlaneIntersection(r, InRayDirection, InRayOrigin, plane.GetNormal(), plane.GetPoint()))
					continue;
				r += InRayDirection * 0.001f;
				const float rDist = (InRayOrigin - r).LengthSquared(); 
				if (rDist > distSqr && distSqr > 0.0f)
					continue;
				if (!IsInside(r))
					continue;
				InResult = r;
				distSqr = rDist;
				hit = true;
			}
			return hit; 
		}
	}
}
