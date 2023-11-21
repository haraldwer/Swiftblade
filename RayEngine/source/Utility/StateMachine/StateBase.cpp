#include "StateBase.h"
#include "StateMachine.h"

Utility::Type StateBase::GetCurrentState() const
{
    CHECK_RETURN_LOG(!Owner, "No owner", Utility::Type::None());
    return Owner->GetCurrentType();
}

StateBase* StateBase::GetState(const Utility::Type& InType) const
{
    CHECK_RETURN_LOG(!Owner, "No owner", nullptr);
    return Owner->GetState(InType);
}

bool StateBase::IsCurrentState() const
{
    return GetCurrentState() == GetType();
}