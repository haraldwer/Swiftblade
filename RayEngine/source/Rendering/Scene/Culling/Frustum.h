#pragma once

struct CameraInstance;

namespace Rendering
{
	class Frustum
	{
	private:
		static float PlaneDotVec(const Vec4F& InPlane, float InX, float InY, float InZ)
		{
			return Vec4F::Dot(InPlane, Vec4F(InX, InY, InZ, 1.0f));
		}

		bool CheckFarDist(float InLengthSqr, float InRadius) const
		{
			const float aFar = Far + InRadius;
			return InLengthSqr < aFar * aFar; // Distance from p to cam is less than far
		}
		
		bool CheckCloseDist(float InLengthSqr, float InRadius) const
		{
			return InLengthSqr < InRadius * InRadius; // Distance from p to cam is less than range
		}

	public:
		void ConstructFrustum(const Mat4F& InProj, const Mat4F& InView, float InFar, Vec3F InPos);
		void ConstructFrustum(const CameraInstance& InCam, const Vec2I& InViewSize);
		
		bool CheckPoint(const Vec3F& InPos) const
		{
			if (Far < 0.01f)
				return true;
			
			const float lSqr = (InPos - Position).LengthSqr();
			if (!CheckFarDist(lSqr, 0))
				return false;
			if (CheckCloseDist(lSqr, 0))
				return true;
			
			// Check if the point is inside all six planes of the view frustum.
			for (const auto& m_plane : Planes)
				if (PlaneDotVec(m_plane, InPos.x, InPos.y, InPos.z) < 0.0f)
					return false;
			return true;
		}

		bool CheckCube(const Vec3F& InPos, float InRadius) const
		{
			if (Far < 0.01f)
				return true;
			
			const float lSqr = (InPos - Position).LengthSqr();
			if (!CheckFarDist(lSqr, InRadius))
				return false;
			if (CheckCloseDist(lSqr, InRadius))
				return true;
			
			// Check if any one point of the cube is in the view frustum.
			for (const auto& plane : Planes)
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

		bool CheckSphere(const Vec3F& InPos, float InRange) const
		{
			if (Far < 0.01f)
				return true;
			
			const float lSqr = (InPos - Position).LengthSqr();
			if (!CheckFarDist(lSqr, InRange))
				return false;
			if (CheckCloseDist(lSqr, InRange))
				return true;
			
			// Check if the radius of the sphere is inside the view frustum.
			for (const auto& plane : Planes)
				if (PlaneDotVec(plane, InPos.x, InPos.y, InPos.z) < -abs(InRange * 2.0f))
					return false;
			return true;
		}

		bool CheckBox(float InXCenter, float InYCenter, float InZCenter, float InXSize, float InYSize, float InZSize) const
		{
			if (Far < 0.01f)
				return true;
			
			// Check if any of the 6 planes of the box are inside the view frustum.
			for (const auto& plane : Planes)
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

		Vec3F GetPosition() const { return Position; }

	private:

		float Far = 0.0f;
		Vec3F Position;
		Vec4F Planes[6];
	};
}
