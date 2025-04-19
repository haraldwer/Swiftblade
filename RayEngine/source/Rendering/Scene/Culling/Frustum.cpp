#include "Frustum.h"

#include "Engine/Rendering/Manager.h"
#include "Scene/Instances/CameraInstance.h"

void Rendering::Frustum::ConstructFrustum(const Mat4F& InProj, const Mat4F& InView, float InFar, Vec3F InPos)
{
	const Mat4F matrix = Mat4F::GetInverse(InView) * InProj;

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

void Rendering::Frustum::ConstructFrustum(const CameraInstance& InCam, const Vec2I& InViewSize)
{
	Mat4F view = InCam.GetViewMatrix();
	Mat4F proj = InCam.GetProjectionMatrix({
		static_cast<float>(InViewSize.x),
		static_cast<float>(InViewSize.y)
	});
	ConstructFrustum(proj, view, InCam.Far, InCam.Position);
}
