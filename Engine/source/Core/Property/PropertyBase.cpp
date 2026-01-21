#include "PropertyBase.h"

#include "PropertyOwner.h"

PropertyBase::PropertyBase(const String& InName) : name(InName)
{
    PropertyOwnerBase::AddProperty(this);
}

const String& PropertyBase::GetName() const
{
    return name;
}
