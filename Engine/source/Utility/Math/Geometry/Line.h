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
			Line() = default;
			Line(const Line& InLine);
			Line(const Vector2<T>& InPoint0, const Vector2<T>& InPoint1);
			void InitWith2Points(const Vector2<T>& InPoint0, const Vector2<T>& InPoint1);
			void InitWithPointAndDirection(const Vector2<T>& InPoint, const Vector2<T>& InDirection);
			bool IsInside(const Vector2<T>& InPosition) const;
			const Vector2<T>& GetDirection() const;
			const Vector2<T>& GetNormal() const;
			const Vector2<T>& GetStart() const;
		private:
			Vector2<T> point0;
			Vector2<T> direction;
			Vector2<T> normal;
		};

		template <class T>
		Line<T>::Line(const Line<T>& InLine)
		{ 
			point0 = InLine.myPoint0;
			direction = InLine.myDirection;
			normal = InLine.myNormal;
		}

		template <class T>
		Line<T>::Line(const Vector2<T>& InPoint0, const Vector2<T>& InPoint1)
		{
			InitWith2Points(InPoint0, InPoint1);
		}

		template<class T>
		void Line<T>::InitWith2Points(const Vector2<T>& InPoint0, const Vector2<T>& InPoint1)
		{
			point0 = InPoint0;
			direction = InPoint1 - InPoint0;
			normal = { -direction.y, direction.x };
		}

		template <class T>
		void Line<T>::InitWithPointAndDirection(const Vector2<T>& InPoint, const Vector2<T>& InDirection)
		{
			point0 = InPoint;
			direction = InDirection;
			normal = { -direction.y, direction.x };
		}

		template<class T>
		bool Line<T>::IsInside(const Vector2<T>& InPosition) const
		{
			return !(GetNormal().Dot(InPosition - point0) > 0);
		}
	
		template<class T>
		const Vector2<T>& Line<T>::GetDirection() const
		{
			return direction;
		}
	
		template<class T>
		const Vector2<T>& Line<T>::GetNormal() const
		{
			return normal;
		}

		template <class T>
		const Vector2<T>& Line<T>::GetStart() const
		{
			return point0;
		}

	}
}
