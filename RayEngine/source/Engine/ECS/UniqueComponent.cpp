#include "UniqueComponent.h"

bool UniqueComponentBase::ShouldUpdate() const
{
    return
        !Engine::Instance::Get().IsEditor() &&
        !Utility::Time::Get().IsPaused();
}
