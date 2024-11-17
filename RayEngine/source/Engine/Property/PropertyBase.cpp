#include "PropertyBase.h"

#include "PropertyOwner.h"

PropertyBase::PropertyBase(const String& InName) : Name(InName)
{
    PropertyOwnerBase::AddProperty(this); 
}

const String& PropertyBase::GetName() const
{
    return Name;
}

bool PropertyBase::operator==(const PropertyBase& InOther) const
{
    return false; 
}
