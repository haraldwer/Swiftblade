#include "Resource.h"

#include "Manager.h"

Resource::Base* Resource::BaseRef::Register(const ID& InID)
{
    Manager& man = Manager::Get();
    auto* res = man.GetResource(InID);
    if (!res)
    {
        res = this->NewRes(InID);
        CHECK_ASSERT(!res, "Failed to resolve virtual override");
        man.Register(res, InID); 
    }
    return res;
}
