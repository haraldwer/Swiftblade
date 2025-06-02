#pragma once
#include "ECS/Volume/Coord.h"
#include "Engine/Blueprints/Blueprint.h"

class PropRuleBase
{
public:
    virtual ~PropRuleBase() = default;
    virtual bool Evaluate(const ECS::CubeVolume& InVolume, Coord InCoord) = 0;
    virtual void Clear();

protected:
    ECS::ID AddProp(const Mat4F& InTrans, const ResBlueprint& InBP);
    
    Vector<ECS::EntityID> generatedProps = {};
};
