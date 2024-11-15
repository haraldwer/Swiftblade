#include "Config.h"

void Input::Context::UpdateCache()
{
    CachedActions.clear();
    auto& actions = Actions.Get();
    for (int i = 0; i < actions.size(); i++)
        CachedActions[actions[i].Name] = i;
}

bool Input::Config::Edit(const String& InName)
{
    if (BaseConfig::Edit(InName))
    {
        UpdateCache();
        return true; 
    }
    return false; 
}

void Input::Config::UpdateCache()
{
    CachedContexts.clear();
    auto& contexts = Contexts.Get();
    for (int i = 0; i < contexts.size(); i++)
    {
        auto& context = contexts[i]; 
        context.UpdateCache(); 
        CachedContexts[context.Name] = i;
    }
}
