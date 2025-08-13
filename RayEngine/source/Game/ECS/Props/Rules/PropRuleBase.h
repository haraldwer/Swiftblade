#pragma once

#include "ECS/Volume/CubeVolumeCoord.h"
#include "Engine/Blueprints/Blueprint.h"

class PropRuleBase
{
public:
    virtual ~PropRuleBase() = default;
    virtual bool Evaluate(const ECS::CubeVolume& InVolume, ECS::VolumeCoord InCoord) = 0;
    virtual void Clear();

protected:
    ECS::ID AddProp(const Mat4F& InTrans, const ResBlueprint& InBP);
    
    Vector<ECS::EntityID> generatedProps = {};
};
