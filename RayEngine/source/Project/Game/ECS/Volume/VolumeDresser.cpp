#include "VolumeDresser.h"

void SysVolumeDresser::Init(ECS::EntityID InID, VolumeDresser& InComponent)
{
    System::Init(InID, InComponent);
}

void SysVolumeDresser::Dress(ECS::EntityID InID)
{
    // Dress!
}

void SysVolumeDresser::Frame(ECS::EntityID InID, VolumeDresser& InComponent)
{
    System::Frame(InID, InComponent);
}

bool SysVolumeDresser::Edit(ECS::EntityID InID)
{
    return System::Edit(InID);
}
