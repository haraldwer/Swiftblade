#include "Animator.h"

#include "Transform.h"

void ECS::SysAnimator::Tick(EntityID InID, Animator &InComponent)
{
    auto anim = InComponent.Anim.Get().Get();
    CHECK_RETURN(!anim);
    InComponent.Time += Utility::Time::Get().Delta();
    Mat4F trans = anim->Get("Cube", InComponent.Time);
    Get<Transform>(InID).SetLocal(trans);
}
