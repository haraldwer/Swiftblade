#pragma once
#include "Engine/ECS/Entity.h"

class GameState : public Utility::Singelton<GameState>
{
friend class GameInstance; 

public: 
    // Deaths
    // Checkpoint
    // Total time

    ECS::EntityID GetPlayerID() const { return playerID; }

    int deaths = 0;
    int checkpoint = 0;
    bool arena = false; 
    double elapsedTime = 0; 
    
private: 
    ECS::EntityID playerID = ECS::INVALID_ID;
};
