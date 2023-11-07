#pragma once

#include "../Vector/Vector2.hpp"

namespace Utility
{
	namespace Math
	{
		template <class T>
		class Line
		{
		public:
			Line();
			Line(const Line<T>& aLine);
			Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
			void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
			void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection);
			bool IsInside(const Vector2<T>& aPosition) const;
			const Vector2<T>& GetDirection() const;
			const Vector2<T>& GetNormal() const;
			const Vector2<T>& GetStart() const;
		private:
			Vector2<T> myPoint0;
			Vector2<T> myDirection;
			Vector2<T> myNormal;
		};

		template <class T>
		Line<T>::Line()
		{
		}

		template <class T>
		Line<T>::Line(const Line<T>& aLine)
		{ 
			myPoint0 = aLine.myPoint0;
			myDirection = aLine.myDirection;
			myNormal = aLine.myNormal;
		}

		template <class T>
		Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
		{
			InitWith2Points(aPoint0, aPoint1);
		}

		template<class T>
		void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
		{
			myPoint0 = aPoint0;
			myDirection = aPoint1 - aPoint0;
			myNormal = { -myDirection.y, myDirection.x };
		}

		template <class T>
		void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
		{
			myPoint0 = aPoint;
			myDirection = aDirection;
			myNormal = { -myDirection.y, myDirection.x };
		}

		template<class T>
		bool Line<T>::IsInside(const Vector2<T>& aPosition) const
		{
			return !(GetNormal().Dot(aPosition - myPoint0) > 0);
		}
	
		template<class T>
		const Vector2<T>& Line<T>::GetDirection() const
		{
			return myDirection;
		}
	
		template<class T>
		const Vector2<T>& Line<T>::GetNormal() const
		{
			return myNormal;
		}

		template <class T>
		const Vector2<T>& Line<T>::GetStart() const
		{
			return myPoint0;
		}

	}
}
