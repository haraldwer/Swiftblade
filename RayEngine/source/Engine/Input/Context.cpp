#include "Context.h"

void Input::Context::UpdateCache()
{
    CachedActions.clear();
    auto& actions = Actions.Get();
    for (int i = 0; i < actions.size(); i++)
        CachedActions[actions[i].Name] = i;
}