#pragma once

namespace ECS
{
    struct Player;
    class SysPlayer;
    class Manager; 
};

namespace Player
{
    class PlayerComp
    {
    protected:

        // Base getters
        ECS::Player& GetPlayer();
        ECS::SysPlayer& GetSystem();
        ECS::Manager& GetManager();

        // Specific getters

        // Can this have properties? 
        
    };
}
