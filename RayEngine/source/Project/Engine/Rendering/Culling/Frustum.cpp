#include "Frustum.h"

#include "Engine/Rendering/Manager.h"
#include "Utility/Math/AngleConversion.h"

void Rendering::Frustum::ConstructFrustum(const Mat4F& InProj, const Mat4F& InView, float InFar, Vec3F InPos)
{
	const Mat4F matrix = InView * InProj;

	auto& near = Planes[0];
	auto& far = Planes[1];
	auto& left = Planes[2];
	auto& right = Planes[3];
	auto& top = Planes[4];
	auto& bottom = Planes[5];
	
	auto& mat = matrix.elements;
	for (int i = 3; i >= 0; i--) left[i]	= mat[i][3] + mat[i][0];
	for (int i = 3; i >= 0; i--) right[i]	= mat[i][3] - mat[i][0];
	for (int i = 3; i >= 0; i--) bottom[i]	= mat[i][3] + mat[i][1];
	for (int i = 3; i >= 0; i--) top[i]		= mat[i][3] - mat[i][1];
	for (int i = 3; i >= 0; i--) near[i]	= mat[i][3] + mat[i][2];
	for (int i = 3; i >= 0; i--) far[i]		= mat[i][3] - mat[i][2];

	Far = InFar;
	Position = InPos;
}

void Rendering::Frustum::ConstructFrustum(const CameraInstance& InCam)
{
	const Mat4F trans = Mat4F(InCam.Position, InCam.Rotation, Vec3F::One());
	const Mat4F view = Mat4F::GetInverse(trans);
	
	Mat4F proj = Mat4F();
	const Vec2F size = Manager::Get().GetViewportSize();
	const float aspect = size.x / size.y;
	const float base = 1.0f / tanf(Utility::Math::DegreesToRadians(InCam.FOV));
	proj(0, 0) = base;
	proj(1, 1) = aspect * base;
	const float dist = InCam.Far / (InCam.Far - InCam.Near);
	proj(2, 2) = dist;
	proj(3, 2) = -InCam.Near * dist;
	proj(2, 3) = 1;
	proj(3, 3) = 0;

	ConstructFrustum(proj, view, InCam.Far, InCam.Position);
}
