#pragma once
#include "Engine/ECS/Entity.h"

class GameState : public Utility::Singelton<GameState>
{
friend class GameInstance; 

public: 
    // Deaths
    // Checkpoint
    // Total time

    ECS::EntityID GetPlayerID() const { return PlayerID; }

    int Seed = 0;
    int Deaths = 0;
    int Checkpoint = 0;
    bool InArena = false; 
    double ElapsedTime = 0; 
    
private: 
    ECS::EntityID PlayerID = ECS::InvalidID;
};
