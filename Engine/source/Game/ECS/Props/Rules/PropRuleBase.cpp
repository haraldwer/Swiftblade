#include "PropRuleBase.h"

#include "Engine/ECS/Manager.h"

void PropRuleBase::Clear()
{
    for (const ECS::EntityID id : generatedProps)
        ECS::Manager::Get().DestroyEntity(id);
    generatedProps.clear();
}

ECS::ID PropRuleBase::AddProp(const Mat4F& InTrans, const ResBlueprint& InBP)
{
    if (const auto bp = InBP.Get())
    {
        const ECS::EntityID id = bp->Instantiate(InTrans);
        generatedProps.push_back(id);
        return id; 
    }
    return ECS::INVALID_ID;
}
