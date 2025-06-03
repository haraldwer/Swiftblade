#include "RoomGenProps.h"

#include "ECS/Volume/CubeVolume.h"
#include "Editor/SubEditors/RoomGenEditor.h"

void RoomGenProps::Clear()
{
    queue.clear();
    for (const auto rule : ruleList)
        rule->Clear();
}

void RoomGenProps::Init()
{
    CHECK_RETURN(!owner);
    PopulateRules();
    const auto& data = owner->GetVolume().data;
    for (const auto& entry : data)
        queue.emplace_back(entry.first);
}

bool RoomGenProps::Step()
{
    CHECK_RETURN(!owner, true);
    
    // Every rule is applied once for every grid position
    // Every rule defines its own behavior

    // Just loop through the entire grid
    // Loop through every neighbor of every block

    const auto& data = owner->GetVolume().data;
    for (int i = 0; i < 50; i++)
    {
        CHECK_BREAK(queue.empty())
        const Coord c = queue.front();
        queue.erase(queue.begin());
        
        if (Evaluate(owner->GetVolume(), c))
            break;
        
        for (auto n : ECS::CubeVolume::GetNeighbors(c))
            if (!data.contains(n.key)) // Only evaluate each coord once
                if (Evaluate(owner->GetVolume(), n))
                    break;
    }
    
    return queue.empty(); 
}

bool RoomGenProps::Evaluate(const ECS::CubeVolume& InVolume, const Coord InCoord) const
{
    for (const auto rule : ruleList)
        if (rule->Evaluate(InVolume, InCoord))
            return true;
    return false;
}
