#include "PropertyOwner.h"

#include "PropertyBase.h"

void PropertyOwner::StartScope()
{
    LOG("Start scope");
    Instance = this;  
}

void PropertyOwner::EndScope()
{
    LOG("End scope");
    Instance = nullptr;
}

void PropertyOwner::AddProperty(PropertyBase* InProperty)
{
    CHECK_ASSERT(!InProperty, "Property null");
    CHECK_ASSERT(!Instance, "No instance");
    Instance->Properties[InProperty->GetName()] = InProperty;
}

void PropertyOwner::Serialize(SerializeObj& InOutObj) const
{
    for (auto& p : Properties)
        p.second->Serialize(InOutObj);
}

void PropertyOwner::Deserialize(const DeserializeObj& InObj)
{
    for (auto& p : Properties)
        p.second->Deserialize(InObj);
}
