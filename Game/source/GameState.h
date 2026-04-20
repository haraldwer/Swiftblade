#pragma once
#include "Engine/ECS/Entity.h"
#include "Level/Level.h"

namespace ECS
{
    class PlayerSpawner;
}

class GameState : public Utility::Singelton<GameState>
{
friend class GameInstance; 
friend class ECS::PlayerSpawner; 

public: 
    // Deaths
    // Checkpoint
    // Total time

    ECS::EntityID GetPlayerID() const { return playerID; }

    int deaths = 0;
    int checkpoint = 0;
    bool arena = false; 
    double elapsedTime = 0;
    LevelConfig level = {};
    
private: 
    ECS::EntityID playerID = ECS::INVALID_ID;
};
