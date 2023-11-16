#include "StateBase.h"
#include "StateMachine.h"

StateBase* StateBase::GetCurrentState() const
{
    CHECK_RETURN_LOG(!Owner, "No owner", nullptr);
    return Owner->GetCurrentState();
}

StateBase* StateBase::GetState(const Utility::TypeAny& InType) const
{
    CHECK_RETURN_LOG(!Owner, "No owner", nullptr);
    return Owner->GetState(InType);
}

bool StateBase::IsCurrentState() const
{
    return GetCurrentState() == this;
}