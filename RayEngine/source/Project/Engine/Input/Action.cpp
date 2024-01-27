#include "Action.h"

#include "Manager.h"

const Input::Action& Input::Action::Get(const String& Action, const String& Context)
{
    if (!Context.empty())
        return Manager::Get().Action(Action, Context);
    if (auto& playerAction = Manager::Get().Action(Action, "Player"))
        return playerAction;
    if (auto& defaultAction = Manager::Get().Action(Action, "Default"))
        return defaultAction;
    return Invalid();
}
