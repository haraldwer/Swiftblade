#include "Material.h"

#include "Instance/Instance.h"
#include "Physics/Manager.h"
#include "Utility/File/File.h"

bool PhysicsMaterialResource::Load()
{
    if (!PropertyFile::Load())
        return false;
    if (Engine::Instance::Get().IsEditor())
        return true;
    
    const auto& man = Physics::Manager::Get();
    ptr = man.CreateMaterial(
        data.StaticFriction,
        data.DynamicFriction,
        data.Restitution);

    return ptr != nullptr;
}

bool PhysicsMaterialResource::Unload()
{
    CHECK_RETURN(!ptr, false);
    ptr = nullptr;
    return PropertyFile::Unload();
}
