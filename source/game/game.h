#pragma once
#include "room.h"
#include "utility/singelton.h"

class Game : public Singelton<Game>
{
    
public:
    void Init() override;
    void Update();
    
    Room Room; 
    
private:
    void FixedUpdate(double InDelta);
    
    static constexpr double TickRate = 0.01; 
    double TickTimer = 0.0;
};
