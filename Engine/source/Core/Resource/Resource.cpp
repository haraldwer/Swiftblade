#include "Resource.h"

#include "Manager.h"

Resource::ImplBase* Resource::BaseRef::Register(const ID& InID)
{
    Manager& man = Manager::Get();
    auto* res = man.GetResource(InID);
    if (!res)
    {
        res = this->New(InID);
        CHECK_ASSERT(!res, "Failed to resolve virtual override");
        man.Register(res, InID); 
    }
    return res;
}
