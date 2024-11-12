#include "C:/Dev/Swiftblade/RayEngine/build/source/Project/CMakeFiles/Project.dir/Debug/cmake_pch.hxx"
#include "PropRuleBase.h"

#include "Engine/ECS/Manager.h"

void PropRuleBase::Clear()
{
    for (const ECS::EntityID id : GeneratedProps)
        ECS::Manager::Get().DestroyEntity(id);
    GeneratedProps.clear();
}

ECS::ID PropRuleBase::AddProp(const Mat4F& InTrans, const ResBlueprint& InBP)
{
    if (auto bp = InBP.Get())
    {
        ECS::EntityID id = bp->Instantiate(InTrans);
        GeneratedProps.push_back(id);
        return id; 
    }
    return ECS::InvalidID;
}
