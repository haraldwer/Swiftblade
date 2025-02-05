#include "AnimationPoser.h"

#include "ECS/Systems/Attributes.h"
#include "ECS/Systems/Transform.h"
#include "Instance/Instance.h"

void ECS::AnimationPoser::Update()
{
    VisualizePoses();
}

bool ECS::AnimationPoser::ShouldUpdate() const
{
    return Engine::Instance::Get().IsEditor();
}

void ECS::AnimationPoser::VisualizePoses() const
{
    Mat4F A;
    Mat4F B;
    for (auto c : GetPoses())
    {
        String name = Get<Attributes>(c).Name.Get();
        if (name.starts_with(VisualizePoseA.Get()) && !VisualizePoseA.Get().empty())
            A = Get<Transform>(c).Local();
        if (name.starts_with(VisualizePoseB.Get()) && !VisualizePoseB.Get().empty())
            B = Get<Transform>(c).Local();
    }

    auto& rs = Engine::Instance::Get().GetRenderScene();
    if (A != Mat4F() || B != Mat4F())
    {
        if (A != Mat4F())
            rs.AddMesh({
                VisualizationMesh,
                VisualizationMat,
                A });
        if (B != Mat4F())
            rs.AddMesh({
                VisualizationMesh,
                VisualizationMat,
                B });
        if (A != Mat4F() && B != Mat4F())
        {
            QuatF rot = QuatF::Slerp(A.GetRotation(), B.GetRotation(), VisualizeLerp.Get());
            Vec3F pos = Lerp(A.GetPosition(), B.GetPosition(), VisualizeLerp.Get());
            rs.AddMesh({
                VisualizationMesh,
                VisualizationMat,
                Mat4F(pos, rot, Vec3F::One()) });
        }
        return;
    }

    for (auto c : GetPoses())
        rs.AddMesh({
            VisualizationMesh,
            VisualizationMat,
            Get<Transform>(c).Local()
        });
}

Mat4F ECS::AnimationPoser::GetPose(const String& InPose) const
{
    auto& t = Get<Transform>();
    for (auto c : GetPoses())
        if (Get<Attributes>(c).Name.Get() == InPose)
            return Get<Transform>(c).Local();
    return {};
}

Vector<ECS::EntityID> ECS::AnimationPoser::GetPoses() const
{
    Vector<EntityID> ids;
    auto& t = Get<Transform>();
    for (auto c : t.GetChildren())
        if (Get<Attributes>(c).Name.Get().starts_with("Pose_"))
            ids.push_back(c);
    return ids;
}
