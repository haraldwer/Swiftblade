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

void ECS::AnimationPoser::VisualizePoses()
{
    Mat4F a;
    Mat4F b;
    for (auto c : GetPoses())
    {
        String name = Get<Attributes>(c).Name.Get();
        if (name.starts_with(VisualizePoseA.Get()) && !VisualizePoseA.Get().empty())
            a = Get<Transform>(c).Local();
        if (name.starts_with(VisualizePoseB.Get()) && !VisualizePoseB.Get().empty())
            b = Get<Transform>(c).Local();
    }
    
    // TODO:
    /*
    auto& rs = Engine::Instance::Get().GetRenderScene();
    if (a != Mat4F() || b != Mat4F())
    {
        if (a != Mat4F())
            rs.Meshes().Add({
                VisualizationMesh,
                VisualizationMat,
                a,
                hash });
        if (b != Mat4F())
            rs.Meshes().Add({
                VisualizationMesh,
                VisualizationMat,
                b,
                hash });
        if (a != Mat4F() && b != Mat4F())
            rs.Meshes().Add({
                VisualizationMesh,
                VisualizationMat,
                Mat4F::Lerp(a, b, VisualizeLerp.Get()),
                hash });
        return;
    }

    for (auto c : GetPoses())
        rs.Meshes().Add({
            VisualizationMesh,
            VisualizationMat,
            Get<Transform>(c).Local(),
            hash 
        });
    */
}

Mat4F ECS::AnimationPoser::GetPose(const String& InPose) const
{
    for (const auto c : GetPoses())
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
