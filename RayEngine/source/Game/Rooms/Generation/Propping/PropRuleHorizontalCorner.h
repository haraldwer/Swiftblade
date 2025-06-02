#pragma once
#include "PropRuleBase.h"

class PropRuleHorizontalCorner : public PropRuleBase
{
public:
    bool Evaluate(const ECS::CubeVolume& InVolume, Coord InCoord) override;

private:
    ResBlueprint cornerBP {"Dressing/Props/BP_HorizontalCorner.json"}; 
};
