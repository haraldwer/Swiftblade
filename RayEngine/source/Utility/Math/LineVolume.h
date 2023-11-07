#pragma once
#include <vector>
#include "Line.h"
namespace Utility
{
	namespace Math
	{
		template <class T>
		class LineVolume
		{
		public:
			LineVolume();
			LineVolume(const std::vector<Line<T>>& aLineList);
			void CalculateRectVolume( Vector2<T> pos, Vector2<T> size, Vector2<T> centering, Vector2<T> scale, T rot, Vector2<T>& worldCenter );
			void AddLine(const Line<T>& aLine);
			bool IsInside(const Vector2<T>& aPosition);
			void Clear();
		private:
			std::vector<Line<T>> myLines;
		};

		template <class T>
		LineVolume<T>::LineVolume()
		{
			myLines = std::vector<Line<T>>();
		}

		template <class T>
		LineVolume<T>::LineVolume(const std::vector<Line<T>>& aLineList)
		{
			myLines.clear();
			for (auto& it : aLineList)
			{
				myLines.push_back(it);
			}
		}

		template <class T>
		void LineVolume<T>::CalculateRectVolume( const Vector2<T> pos,
			const Vector2<T> size,
			const Vector2<T> centering,
			const Vector2<T> scale,
			const T rot,
			Vector2<T>& worldCenter )
		{
			myLines.clear();
			const T cosRot = cos( rot );
			const T sinRot = sin( rot );

			const T x1 = size.x - centering.x;
			const T y1 = size.y - centering.y;
			const T x2 = -centering.x;
			const T y2 = -centering.y;

			const Vector2<T> pos1 = { ( (x1)*cosRot * scale.x + (y1)*sinRot * scale.y ) + pos.x, ( (y1)*cosRot * scale.y - (x1)*sinRot * scale.x ) + pos.y };
			const Vector2<T> pos2 = { ( (x2)*cosRot * scale.x + (y1)*sinRot * scale.y ) + pos.x, ( (y1)*cosRot * scale.y - (x2)*sinRot * scale.x ) + pos.y };
			const Vector2<T> pos3 = { ( (x2)*cosRot * scale.x + (y2)*sinRot * scale.y ) + pos.x, ( (y2)*cosRot * scale.y - (x2)*sinRot * scale.x ) + pos.y };
			const Vector2<T> pos4 = { ( (x1)*cosRot * scale.x + (y2)*sinRot * scale.y ) + pos.x, ( (y2)*cosRot * scale.y - (x1)*sinRot * scale.x ) + pos.y };

			worldCenter = pos1 + pos2 + pos3 + pos4;
			worldCenter *= 0.25f;

			myLines.push_back( Line<T>( pos2, pos1 ) );
			myLines.push_back( Line<T>( pos3, pos2 ) );
			myLines.push_back( Line<T>( pos4, pos3 ) );
			myLines.push_back( Line<T>( pos1, pos4 ) );
		}

		template <class T>
		void LineVolume<T>::AddLine(const Line<T>& aLine)
		{
			myLines.push_back(aLine);
		}

		template <class T>
		bool LineVolume<T>::IsInside(const Vector2<T>& aPosition)
		{
			for(auto& it : myLines)
			{
				if(!it.IsInside(aPosition))
				{
					return false;
				}
			}
			return true;
		}

		template <class T>
		void LineVolume<T>::Clear()
		{
			myLines.clear();
		}

	}
}
