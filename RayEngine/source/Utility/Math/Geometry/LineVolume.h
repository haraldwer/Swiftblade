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
			LineVolume(const std::vector<Line<T>>& InLines);
			void CalculateRectVolume( Vector2<T> InPos, Vector2<T> InSize, Vector2<T> InCentering, Vector2<T> InScale, T InRot, Vector2<T>& InWorldCenter );
			void AddLine(const Line<T>& InLine);
			bool IsInside(const Vector2<T>& InPosition);
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
		LineVolume<T>::LineVolume(const std::vector<Line<T>>& InLines)
		{
			myLines.clear();
			for (auto& it : InLines)
			{
				myLines.push_back(it);
			}
		}

		template <class T>
		void LineVolume<T>::CalculateRectVolume( const Vector2<T> InPos,
			const Vector2<T> InSize,
			const Vector2<T> InCentering,
			const Vector2<T> InScale,
			const T InRot,
			Vector2<T>& InWorldCenter )
		{
			myLines.clear();
			const T cosRot = cos( InRot );
			const T sinRot = sin( InRot );

			const T x1 = InSize.x - InCentering.x;
			const T y1 = InSize.y - InCentering.y;
			const T x2 = -InCentering.x;
			const T y2 = -InCentering.y;

			const Vector2<T> pos1 = { ( (x1)*cosRot * InScale.x + (y1)*sinRot * InScale.y ) + InPos.x, ( (y1)*cosRot * InScale.y - (x1)*sinRot * InScale.x ) + InPos.y };
			const Vector2<T> pos2 = { ( (x2)*cosRot * InScale.x + (y1)*sinRot * InScale.y ) + InPos.x, ( (y1)*cosRot * InScale.y - (x2)*sinRot * InScale.x ) + InPos.y };
			const Vector2<T> pos3 = { ( (x2)*cosRot * InScale.x + (y2)*sinRot * InScale.y ) + InPos.x, ( (y2)*cosRot * InScale.y - (x2)*sinRot * InScale.x ) + InPos.y };
			const Vector2<T> pos4 = { ( (x1)*cosRot * InScale.x + (y2)*sinRot * InScale.y ) + InPos.x, ( (y2)*cosRot * InScale.y - (x1)*sinRot * InScale.x ) + InPos.y };

			InWorldCenter = pos1 + pos2 + pos3 + pos4;
			InWorldCenter *= 0.25f;

			myLines.push_back( Line<T>( pos2, pos1 ) );
			myLines.push_back( Line<T>( pos3, pos2 ) );
			myLines.push_back( Line<T>( pos4, pos3 ) );
			myLines.push_back( Line<T>( pos1, pos4 ) );
		}

		template <class T>
		void LineVolume<T>::AddLine(const Line<T>& InLine)
		{
			myLines.push_back(InLine);
		}

		template <class T>
		bool LineVolume<T>::IsInside(const Vector2<T>& InPosition)
		{
			for(auto& it : myLines)
			{
				if(!it.IsInside(InPosition))
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
