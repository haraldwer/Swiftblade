#include "Animator.h"

#include "Transform.h"

void ECS::SysAnimator::Update(EntityID InID, Animator &InComponent)
{
    auto anim = InComponent.Anim.Get().Get();
    CHECK_RETURN(!anim);
    Mat4F trans = anim->Get("Cube", static_cast<float>(Utility::Time::Get().Total()));
    Get<Transform>(InID).SetLocal(trans);
}
