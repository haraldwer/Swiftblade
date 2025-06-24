#include "PropertyBase.h"

#include "PropertyOwner.h"

PropertyBase::PropertyBase(const String& InName) : name(InName)
{
    if (PropertyOwnerBase::AddProperty(this))
        LOG("Property: " + InName);
}

const String& PropertyBase::GetName() const
{
    return name;
}
