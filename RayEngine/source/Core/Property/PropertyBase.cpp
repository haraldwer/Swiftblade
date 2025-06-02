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

bool PropertyBase::operator==(const PropertyBase& InOther) const
{
    return false; 
}
