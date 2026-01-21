#include "StateBase.h"
#include "StateMachine.h"

Utility::Type StateBase::GetCurrentState() const
{
    CHECK_RETURN_LOG(!owner, "No owner", Utility::Type::None());
    return owner->GetCurrentType();
}

StateBase* StateBase::GetState(const Utility::Type& InType) const
{
    CHECK_RETURN_LOG(!owner, "No owner", nullptr);
    return owner->GetState(InType);
}

bool StateBase::IsCurrentState() const
{
    return GetCurrentState() == GetType();
}