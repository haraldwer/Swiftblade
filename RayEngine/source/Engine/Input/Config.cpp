#include "Config.h"

void Input::Context::UpdateCache()
{
    CachedActions.clear();
    auto& actions = Actions.Get();
    for (size_t i = 0; i < actions.size(); i++)
        CachedActions[actions[i].Name] = static_cast<int32>(i);
}

bool Input::Config::Edit(const String& InName, const uint32 InOffset)
{
    if (BaseConfig::Edit(InName, InOffset))
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
    for (size_t i = 0; i < contexts.size(); i++)
    {
        auto& context = contexts[i]; 
        context.UpdateCache(); 
        CachedContexts[context.Name] = static_cast<int32>(i);
    }
}
