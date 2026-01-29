#include "UniqueComponent.h"

#include "Instance/Instance.h"

bool ECS::UniqueComponentBase::ShouldTick() const
{
    return
        !Engine::Instance::Get().IsEditor() &&
        !Utility::Time::Get().IsPaused();
}
