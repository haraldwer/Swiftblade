#pragma once

#include "ECS/System.h"
#include "ECS/Component.h"
#include "Resources/Animation.h"

namespace ECS
{
    struct Animator : Component<Animator>
    {
        PROPERTY_C(ResAnim, Anim, "A_test.json");
        float Time = 0.0f;
    };

    class SysAnimator : public System<Animator>
    {
    public:
        void Update(EntityID InID, Animator &InComponent) override;
        bool ShouldUpdate() const override { return true; }
    };
}
