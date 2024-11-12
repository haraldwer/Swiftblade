#include "RoomGenProps.h"

void RoomGenProps::Clear()
{
    Queue.clear();
    for (auto rule : RuleList)
        rule->Clear();
}

void RoomGenProps::Init()
{
    CHECK_RETURN(!Owner);
    PopulateRules();
    auto& data = Owner->GetVolume().Data;
    for (auto& entry : data)
        Queue.push_back(entry.first);
}

bool RoomGenProps::Step()
{
    CHECK_RETURN(!Owner, true);
    
    // Every rule is applied once for every grid position
    // Every rule defines its own behavior

    // Just loop through the entire grid
    // Loop through every neighbor of every block

    auto& data = Owner->GetVolume().Data;
    for (int i = 0; i < 50; i++)
    {
        CHECK_BREAK(Queue.empty())
        Coord c = Queue.front();
        Queue.erase(Queue.begin());
        
        if (Evaluate(Owner->GetVolume(), c))
            break;
        
        for (auto n : ECS::CubeVolume::GetNeighbors(c))
            if (!data.contains(n.Key)) // Only evaluate each coord once
                if (Evaluate(Owner->GetVolume(), n))
                    break;
    }
    
    return Queue.empty(); 
}

bool RoomGenProps::Evaluate(const ECS::CubeVolume& InVolume, Coord InCoord) const
{
    for (auto rule : RuleList)
        if (rule->Evaluate(InVolume, InCoord))
            return true;
    return false;
}
