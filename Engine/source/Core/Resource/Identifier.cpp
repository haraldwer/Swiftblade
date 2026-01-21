#include "Identifier.h"

void Resource::ID::Serialize(SerializeObj &InObj)
{
    CHECK_RETURN(Unique() || !IsValid())
    Utility::WriteValue(InObj, str);
}

bool Resource::ID::Deserialize(const DeserializeObj &InObj)
{
    bool r = Utility::ReadValue(InObj, str);
    hash = Utility::Hash(str);
    return r;
}