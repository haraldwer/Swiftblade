#pragma once
#include "PropRuleBase.h"

class PropRuleVerticalCorner : public PropRuleBase
{
public:
    bool Evaluate(const ECS::CubeVolume& InVolume, ECS::VolumeCoord InCoord) override;

private:
    ResBlueprint ledgeBP {"Dressing/Props/BP_VerticalCorner.json"}; 
};
