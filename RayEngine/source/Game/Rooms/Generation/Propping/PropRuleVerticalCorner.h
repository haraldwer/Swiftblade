#pragma once
#include "PropRuleBase.h"

class PropRuleVerticalCorner : public PropRuleBase
{
public:
    bool Evaluate(const ECS::CubeVolume& InVolume, Coord InCoord) override;

private:
    ResBlueprint LedgeBP {"Dressing/Props/BP_VerticalCorner.json"}; 
};
