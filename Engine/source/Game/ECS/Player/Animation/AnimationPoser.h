#pragma once

#include "ECS/UniqueComponent.h"
#include "Resources/Material.h"
#include "Resources/Model.h"

namespace ECS
{
    class AnimationPoser : public UniqueComponent<AnimationPoser>
    {
    public:
        void Update() override;
        bool ShouldUpdate() const override;
        Mat4F GetPose(const String& InPose) const;

    private:
        
        Vector<EntityID> GetPoses() const;
        void VisualizePoses();
        
        PROPERTY_D(ResModel, VisualizationMesh, "Defaults/M_Cube.obj");
        PROPERTY_D(ResRM, VisualizationMat, "Defaults/RM_Default.json");
        
        PROPERTY_C(String, VisualizePoseA, "");
        PROPERTY_C(String, VisualizePoseB, "");
        PROPERTY_C(float, VisualizeLerp, 0.5f);
        
    };
}
