#pragma once

namespace Utility
{
    class Time : public Singelton<Time>
    {
    public:

        void Tick(const double InDelta = 0.0)
        {
            delta = InDelta;
        }
        
        void SetScale(const double InScale)
        {
            timeScale = InScale;
        }
        
        double Total() const { return totalTimer.Ellapsed(); }
        
        double Delta() const { return delta * timeScale * static_cast<float>(!paused); }
        
        double Unscaled() const { return delta; }
        
        double Scale() const { return timeScale; }

        void SetPause(const bool InPause)
        {
            paused = InPause; 
        }
        
        bool IsPaused() const { return paused; }
    
    private:
        
        Timer totalTimer;
        double delta = 0.0;
        double timeScale = 1.0f;
        bool paused = false; 
    };
}
