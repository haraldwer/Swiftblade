#include "Action.h"

#include "Manager.h"

const Input::Action& Input::Action::Get(const String& Action, const String& Context)
{
    return Manager::Get().GetAction(Action, Context);
}
