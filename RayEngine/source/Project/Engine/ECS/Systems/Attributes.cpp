#include "Attributes.h"

void ECS::SysAttributes::Init(EntityID InID, Attributes& InComponent)
{
    if (InComponent.Name.Get().empty())
        InComponent.Name = "Unnamed" + std::to_string(InID); 
}
