#pragma once
#include "Utility/Singelton.h"
#include "Timer.h"

namespace Utility
{
    class Time : public Singelton<Time>
    {
    public:

        void Tick(double InDelta = 0.0)
        {
            LastDelta = InDelta;
        }
        
        void SetScale(double InScale)
        {
            TimeScale = InScale;
        }
        
        double Total() const { return TotalTimer.Ellapsed(); }
        double Delta() const { return LastDelta * TimeScale * static_cast<float>(!Paused); }
        double Unscaled() const { return LastDelta; }
        double Scale() const { return TimeScale; }

        void SetPause(const bool InPause)
        {
            Paused = InPause; 
        }
        
        bool IsPaused() const { return Paused; }
        
    private:
        
        Timer TotalTimer;
        
        double LastDelta = 0.0;
        double TimeScale = 1.0f;
        bool Paused = false; 
    };
}
