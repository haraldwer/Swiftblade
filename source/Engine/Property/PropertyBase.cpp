#include "PropertyBase.h"

#include "PropertyOwner.h"

PropertyBase::PropertyBase(const String& InName): Name(InName)
{
    PropertyOwner::AddProperty(this); 
}
