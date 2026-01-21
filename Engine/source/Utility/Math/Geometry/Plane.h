#pragma once

#include "../Vector/Vector3.hpp"

namespace Utility::Math
{
	template <class T>
	class Plane
	{
	public:
		Plane() = default;
		
		// Constructor taking three points where the normal is (aPoint1 - aPoint0) x (aPoint2 - aPoint0).
		Plane(const Vector3<T>& InPoint0, const Vector3<T>& InPoint1, const Vector3<T>& InPoint2);
		// Constructor taking a point and a normal.
		Plane(const Vector3<T>& InOrigin, const Vector3<T>& InNormal);
		// Constructor taking only normal
		explicit Plane(const Vector3<T>& InNormal);
		
		// Returns whether a point is inside the plane: it is inside when the point is on the plane or on the side the normal is pointing away from.
		bool IsInside(const Vector3<T> & InPos) const;
		// Projects a point onto the plane.
		Vector3<T> Project(Vector3<T> InPoint);
		// Calculates the distance from a point to the closest point on the plane. Sign specifies if inside or outside
		float SignedDistance(Vector3<T> InPoint);
		
		// Returns the normal of the plane.
		const Vector3<T>& GetNormal() const;
		// Returns the point of the plane.
		const Vector3<T>& GetPoint() const;
		
		
	private:
		Vector3<T> origin = {};
		Vector3<T> normal = {};
	};

	template <class T>
	Plane<T>::Plane(const Vector3<T>& InPoint0, const Vector3<T>& InPoint1, const Vector3<T>& InPoint2)
	{
		origin = InPoint0;
		normal = (InPoint1 - InPoint0).Cross(InPoint2 - InPoint0);
	}

	template <class T>
	Plane<T>::Plane(const Vector3<T>& InOrigin, const Vector3<T>& InNormal)
	{
		origin = InOrigin;
		normal = InNormal;
	}

	template <class T>
	Plane<T>::Plane(const Vector3<T>& InNormal)
	{
		origin = Vector3<T>::Zero();
		normal = InNormal; 
	}

	template <class T>
	bool Plane<T>::IsInside(const Vector3<T>& InPos) const
	{
		return !(Vector3<T>::Dot(GetNormal(), InPos - origin) > 0);
	}
	
	template <class T>
	Vector3<T> Plane<T>::Project(Vector3<T> InPoint)
	{
		const float dist = SignedDistance(InPoint);
		return InPoint - dist * normal;
	}

	template <class T>
	float Plane<T>::SignedDistance(Vector3<T> InPoint)
	{
		const Vector3<T> diff = InPoint - origin;
		return Vector3<T>::Dot(normal, diff);
	}

	template<class T>
	const Vector3<T>& Plane<T>::GetNormal() const
	{
		return normal;
	}

	template <class T>
	const Vector3<T>& Plane<T>::GetPoint() const
	{
		return origin;
	}
}

