#include "Material.h"

#include <physx/PxMaterial.h>

#include "Instance/Instance.h"
#include "Physics/Manager.h"
#include "Utility/File/File.h"

bool PhysicsMaterialResource::Load(const String& InPath)
{
    identifier = InPath;
    if (!PropertyOwnerBase::Load(InPath))
        return false;

    if (Engine::Instance::Get().IsEditor())
        return true; 
    
    const auto& man = Physics::Manager::Get();
    ptr = man.CreateMaterial(
        StaticFriction,
        DynamicFriction,
        Restitution);

    return ptr != nullptr;
}

bool PhysicsMaterialResource::Unload()
{
    CHECK_RETURN(!ptr, false);
    ptr->release();
    ptr = nullptr;
    return true;
}

Utility::Timepoint PhysicsMaterialResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(identifier);  
}
