#include "PropertyOwner.h"

#include "PropertyBase.h"

void PropertyOwner::StartScope()
{
    LOG("Start scope");
    InstanceStack.push_back(this); 
}

void PropertyOwner::EndScope()
{
    LOG("End scope");
    InstanceStack.pop_back();
}

void PropertyOwner::AddProperty(PropertyBase* InProperty)
{
    CHECK_ASSERT(!InProperty, "Property null");
    CHECK_ASSERT(InstanceStack.empty(), "InstanceStack empty");
    InstanceStack[InstanceStack.size() - 1]->Properties[InProperty->GetName()] = InProperty;
}

String PropertyOwner::Serialize() const
{
    String s; 
    for (auto& p : Properties)
        if (p.second)
            s += p.second->Serialize();
    return s; 
}

void PropertyOwner::Deserialize(const String& InString)
{
    for (auto& p : Properties)
        if (p.second)
            p.second->Deserialize(InString); 
}
