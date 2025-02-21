#include "Material.h"

#include <physx/PxMaterial.h>

#include "Instance/Instance.h"
#include "Physics/Manager.h"
#include "Utility/File/File.h"

bool PhysicsMaterialResource::Load(const String& InPath)
{
    Identifier = InPath;
    if (!PropertyOwnerBase::Load(InPath))
        return false;

    if (Engine::Instance::Get().IsEditor())
        return true; 
    
    const auto& man = Physics::Manager::Get();
    Ptr = man.CreateMaterial(
        StaticFriction,
        DynamicFriction,
        Restitution);

    return Ptr != nullptr;
}

bool PhysicsMaterialResource::Unload()
{
    CHECK_RETURN(!Ptr, false);
    Ptr->release();
    Ptr = nullptr;
    return true;
}

Utility::Timepoint PhysicsMaterialResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(Identifier);  
}
