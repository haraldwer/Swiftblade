#pragma once

#include "ECS/Volume/Coord.h"
#include "Propping/PropRuleHorizontalCorner.h"
#include "Propping/PropRuleVerticalCorner.h"
#include "RoomGenBase.h"

class RoomGenProps : public RoomGenBase
{
public:
    void Clear() override;
    void Init() override;
    bool Step() override;
    
private:
    
    bool Evaluate(const ECS::CubeVolume& InVolume, Coord InCoord) const;
    Vector<Coord> queue = {};
    
    Vector<PropRuleBase*> ruleList = {};
    PropRuleHorizontalCorner ruleCornerHorizontal = {};
    PropRuleVerticalCorner ruleCornerVertical = {};
    
    void PopulateRules()
    {
        ruleList = {
            &(ruleCornerHorizontal),
            &(ruleCornerVertical)
        };
    }
};
