#include "Material.h"

#include <physx/PxMaterial.h>

#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Utility/File/File.h"

bool PhysicsMaterialResource::Load(const String& InPath)
{
    if (!PropertyOwnerBase::Load(InPath))
        return false;

    if (Engine::Instance::Get().IsEditor())
        return false; 
    
    const auto& man = Physics::Manager::Get();
    Ptr = man.CreateMaterial(
        StaticFriction,
        DynamicFriction,
        Restitution);

    Identifier = InPath;
    return Ptr;
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
