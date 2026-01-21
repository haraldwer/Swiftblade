#include "UniqueComponent.h"

#include "Instance/Instance.h"

bool ECS::UniqueComponentBase::ShouldUpdate() const
{
    return
        !Engine::Instance::Get().IsEditor() &&
        !Utility::Time::Get().IsPaused();
}
