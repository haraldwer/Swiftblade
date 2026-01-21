#include "Action.h"

#include "Manager.h"

const Input::Action& Input::Action::Get(const String& InAction, const String& InContext)
{
    return Manager::Get().GetAction(InAction, InContext);
}
