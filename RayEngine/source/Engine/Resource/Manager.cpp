#include "Manager.h"

#include "Impl.h"

Resource::Base* Resource::Manager::GetResource(const String& InIdentifier)
{
    const auto find = Resources.find(InIdentifier);
    if (find == Resources.end())
        return nullptr; 
    return find->second;
}

void Resource::Manager::Register(Base* InResource, const String& InIdentifier)
{
    Resources[InIdentifier] = InResource;
}

void Resource::Manager::Update() const
{
    for (const auto& res : Resources)
    {
        CHECK_CONTINUE(!res.second)
        if (res.second->Count <= 0)
        {
            res.second->Unload();
            LOG("Resource unloaded: " + res.second->Identifier);
        }
        else
        {
            // Should reload?
            res.second->TryHotReload();
        }
    }
}

void Resource::Manager::Deinit()
{
    // TODO: Unload all
    for (auto& res : Resources)
    {
        CHECK_CONTINUE(!res.second)
        res.second->Unload();
        CHECK_CONTINUE_LOG(res.second->Count != 0, ("Resource couldnt be destroyed properly, count: " + std::to_string(res.second->Count)));
        delete(res.second);
        res.second = nullptr;
    }
    Resources.clear();
}
