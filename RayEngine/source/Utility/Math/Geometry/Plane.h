#pragma once

#include "../Vector/Vector3.hpp"

namespace Utility
{
	namespace Math
	{
		template <class T>
		class Plane
		{
		public:
			Plane();
			// Constructor taking three points where the normal is (aPoint1 - aPoint0) x (aPoint2 - aPoint0).
			Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
			// Constructor taking a point and a normal.
			Plane(const Vector3<T>& aPoint, const Vector3<T>& aNormal);
			// Constructor taking only normal
			Plane(const Vector3<T>& aNormal);
			
			// Returns whether a point is inside the plane: it is inside when the point is on the plane or on the side the normal is pointing away from.
			bool IsInside(const Vector3<T> & aPosition) const;
			// Projects a point onto the plane.
			Vector3<T> Project(Vector3<T> InPoint);
			// Calculates the distance from a point to the closest point on the plane. Sign specifies if inside or outside
			float SignedDistance(Vector3<T> InPoint);
			
			// Returns the normal of the plane.
			const Vector3<T>& GetNormal() const;
			// Returns the point of the plane.
			const Vector3<T>& GetPoint() const;
			
			
		private:
			Vector3<T> myPoint0;
			Vector3<T> myNormal;
		};

		template <class T>
		Plane<T>::Plane()
		{
		}

		template <class T>
		Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
		{
			myPoint0 = aPoint0;
			myNormal = (aPoint1 - aPoint0).Cross(aPoint2 - aPoint0);
		}

		template <class T>
		Plane<T>::Plane(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
		{
			myPoint0 = aPoint;
			myNormal = aNormal;
		}

		template <class T>
		Plane<T>::Plane(const Vector3<T>& aNormal)
		{
			myPoint0 = Vector3<T>::Zero();
			myNormal = aNormal; 
		}

		template <class T>
		bool Plane<T>::IsInside(const Vector3<T>& aPosition) const
		{
			return !(GetNormal().Dot(aPosition - myPoint0) > 0);
		}
		
		template <class T>
		Vector3<T> Plane<T>::Project(Vector3<T> InPoint)
		{
			const float dist = SignedDistance(InPoint);
			return InPoint - dist * myNormal;
		}

		template <class T>
		float Plane<T>::SignedDistance(Vector3<T> InPoint)
		{
			const Vector3<T> diff = InPoint - myPoint0;
			return Vector3<T>::Dot(myNormal, diff);
		}

		template<class T>
		const Vector3<T>& Plane<T>::GetNormal() const
		{
			return myNormal;
		}

		template <class T>
		const Vector3<T>& Plane<T>::GetPoint() const
		{
			return myPoint0;
		}
	}
}
