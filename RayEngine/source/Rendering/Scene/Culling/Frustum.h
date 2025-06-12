#pragma once
#include "Collections/SplitContainer.h"

struct CameraInstance;

namespace Rendering
{
	class Frustum
	{
	private:
		static float PlaneDotVec(const Vec4F& InPlane, const float InX, const float InY, const float InZ)
		{
			return Vec4F::Dot(InPlane, Vec4F(InX, InY, InZ, 1.0f));
		}

		bool CheckFarDist(const float InLengthSqr, const float InRadius) const
		{
			const float aFar = far + InRadius;
			return InLengthSqr < aFar * aFar; // Distance from p to cam is less than far
		}

		static bool CheckCloseDist(const float InLengthSqr, const float InRadius)
		{
			return InLengthSqr < InRadius * InRadius; // Distance from p to cam is less than range
		}

	public:
		
		void ConstructFrustum(const Mat4F& InProj, const Mat4F& InView, float InFar, Vec3F InPos);
		void ConstructFrustum(const CameraInstance& InCam, const Vec2I& InViewSize);

		bool CheckPoint(const Vec3F& InPos) const
		{
			if (far < 0.01f)
				return true;
			
			const float lSqr = (InPos - position).LengthSqr();
			if (!CheckFarDist(lSqr, 0))
				return false;
			if (CheckCloseDist(lSqr, 0))
				return true;
			
			// Check if the point is inside all six planes of the view frustum.
			for (const auto& mPlane : planes)
				if (PlaneDotVec(mPlane, InPos.x, InPos.y, InPos.z) < 0.0f)
					return false;
			return true;
		}

		bool CheckCube(const Vec3F& InPos, const float InRadius) const
		{
			if (far < 0.01f)
				return true;
			
			const float lSqr = (InPos - position).LengthSqr();
			if (!CheckFarDist(lSqr, InRadius))
				return false;
			if (CheckCloseDist(lSqr, InRadius))
				return true;
			
			// Check if any one point of the cube is in the view frustum.
			for (const auto& plane : planes)
			{
				if (PlaneDotVec(plane, InPos.x, InPos.y, InPos.z) >= -abs(InRadius * 2.0f) || // Sphere check
					PlaneDotVec(plane, (InPos.x - InRadius), (InPos.y - InRadius), (InPos.z - InRadius)) >= 0.0f ||
					PlaneDotVec(plane, (InPos.x + InRadius), (InPos.y - InRadius), (InPos.z - InRadius)) >= 0.0f ||
					PlaneDotVec(plane, (InPos.x - InRadius), (InPos.y + InRadius), (InPos.z - InRadius)) >= 0.0f ||
					PlaneDotVec(plane, (InPos.x + InRadius), (InPos.y + InRadius), (InPos.z - InRadius)) >= 0.0f ||
					PlaneDotVec(plane, (InPos.x - InRadius), (InPos.y - InRadius), (InPos.z + InRadius)) >= 0.0f ||
					PlaneDotVec(plane, (InPos.x + InRadius), (InPos.y - InRadius), (InPos.z + InRadius)) >= 0.0f ||
					PlaneDotVec(plane, (InPos.x - InRadius), (InPos.y + InRadius), (InPos.z + InRadius)) >= 0.0f ||
					PlaneDotVec(plane, (InPos.x + InRadius), (InPos.y + InRadius), (InPos.z + InRadius)) >= 0.0f)
					continue;

				return false;
			}
			return true;
		}

		bool CheckSphere(const Vec3F& InPos, const float InRange) const
		{
			if (far < 0.01f)
				return true;
			
			const float lSqr = (InPos - position).LengthSqr();
			if (!CheckFarDist(lSqr, InRange))
				return false;
			if (CheckCloseDist(lSqr, InRange))
				return true;
			
			// Check if the radius of the sphere is inside the view frustum.
			for (const auto& plane : planes)
				if (PlaneDotVec(plane, InPos.x, InPos.y, InPos.z) < -abs(InRange * 2.0f))
					return false;
			return true;
		}

		bool CheckBox(const float InXCenter, const float InYCenter, const float InZCenter, const float InXSize, const float InYSize, const float InZSize) const
		{
			if (far < 0.01f)
				return true;
			
			// Check if any of the 6 planes of the box are inside the view frustum.
			for (const auto& plane : planes)
			{
				if (PlaneDotVec(plane, (InXCenter - InXSize), (InYCenter - InYSize), (InZCenter - InZSize)) >= 0.0f ||
					PlaneDotVec(plane, (InXCenter + InXSize), (InYCenter - InYSize), (InZCenter - InZSize)) >= 0.0f ||
					PlaneDotVec(plane, (InXCenter - InXSize), (InYCenter + InYSize), (InZCenter - InZSize)) >= 0.0f ||
					PlaneDotVec(plane, (InXCenter - InXSize), (InYCenter - InYSize), (InZCenter + InZSize)) >= 0.0f ||
					PlaneDotVec(plane, (InXCenter + InXSize), (InYCenter + InYSize), (InZCenter - InZSize)) >= 0.0f ||
					PlaneDotVec(plane, (InXCenter + InXSize), (InYCenter - InYSize), (InZCenter + InZSize)) >= 0.0f ||
					PlaneDotVec(plane, (InXCenter - InXSize), (InYCenter + InYSize), (InZCenter + InZSize)) >= 0.0f ||
					PlaneDotVec(plane, (InXCenter + InXSize), (InYCenter + InYSize), (InZCenter + InZSize)) >= 0.0f)
					continue;
				return false;
			}
			return true;
		}

		Vec3F GetPosition() const { return position; }

	private:

		float far = 0.0f;
		Vec3F position = {};
		Array<Vec4F, 6> planes = {};
	};
}
