#include "Action.h"

#include "Manager.h"

const Input::Action& Input::Action::Get(const String& Action, const String& Context)
{
    if (!Context.empty())
    {
        auto& action = Manager::Get().Action(Action, Context);
        if (!action)
            LOG("Unable to find input action " + Action + " in context " + Context);
        return Manager::Get().Action(Action, Context);
    }
    if (auto& playerAction = Manager::Get().Action(Action, "Player"))
        return playerAction;
    if (auto& defaultAction = Manager::Get().Action(Action, "Default"))
        return defaultAction;
    LOG("Unable to find input action " + Action);
    return Invalid();
}
