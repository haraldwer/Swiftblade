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
            LastFrame = InDelta > 0.00000000001 ?
                InDelta : DeltaTimer.Ellapsed();
            DeltaTimer = Timer(); 
        }
        
        void SetScale(double InScale)
        {
            TimeScale = InScale;
        }
        
        double Total() const { return TotalTimer.Ellapsed(); }
        double Delta() const { return LastFrame * TimeScale * static_cast<float>(!Paused); }
        double Unscaled() const { return LastFrame; }
        double Scale() const { return TimeScale; }

        void SetPause(const bool InPause)
        {
            Paused = InPause; 
        }
        
        bool IsPaused() const { return Paused; }
        
    private:
        
        Timer TotalTimer;
        Timer DeltaTimer;
        
        double LastFrame = 0.0;
        double TimeScale = 1.0f;
        bool Paused = false; 
        
    };
}
