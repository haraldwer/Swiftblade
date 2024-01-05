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
			PlaneVolume(const std::vector<Plane<T>>& aPlaneList);
			void AddPlane(const Plane<T>& aPlane);
			bool IsInside(const Vector3<T>& aPosition);
			bool FindIntersect(Vector3<T>& aResult, const Vector3<T>& aRayOirign, const Vector3<T>& aRayDirection);
		private:
			std::vector<Plane<T>> myPlanes;
		};

		template <class T>
		PlaneVolume<T>::PlaneVolume()
		{
			myPlanes = std::vector<Plane<T>>();
		}
	
		template<class T>
		inline PlaneVolume<T>::PlaneVolume(const std::vector<Plane<T>>& aPlaneList)
		{
			myPlanes.clear();
			for (auto& it : aPlaneList)
			{
				myPlanes.push_back(it);
			}
		}
	
		template<class T>
		inline void PlaneVolume<T>::AddPlane(const Plane<T>& aPlane)
		{
			myPlanes.push_back(aPlane);
		}
	
		template<class T>
		inline bool PlaneVolume<T>::IsInside(const Vector3<T>& aPosition)
		{
			for(auto& it : myPlanes)
			{
				if(!it.IsInside(aPosition))
				{
					return false;
				}
			}
			return true;
		}

		template <class T>
		bool PlaneVolume<T>::FindIntersect(Vector3<T>& aResult, const Vector3<T>& aRayOirign, const Vector3<T>& aRayDirection)
		{
			if (IsInside(aRayOirign))
			{
				aResult = aRayOirign; 
				return true; 
			}
			
			float distSqr = -1.0f;
			bool hit = false; 
			for (auto& plane : myPlanes)
			{
				Vector3<T> r; 
				if (!LinePlaneIntersection(r, aRayDirection, aRayOirign, plane.GetNormal(), plane.GetPoint()))
					continue;
				r += aRayDirection * 0.001f;
				const float rDist = (aRayOirign - r).LengthSquared(); 
				if (rDist > distSqr && distSqr > 0.0f)
					continue;
				if (!IsInside(r))
					continue;
				aResult = r;
				distSqr = rDist;
				hit = true;
			}
			return hit; 
		}
	}
}
