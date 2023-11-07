#pragma once
#include <vector>
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

	}
}
