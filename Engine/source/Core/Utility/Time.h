#pragma once

namespace Utility
{
    class Time : public Singelton<Time>
    {
    public:

        void Tick(const double InDelta = 0.0)
        {
            lastDelta = InDelta;
        }
        
        void SetScale(const double InScale)
        {
            timeScale = InScale;
        }
        
        double Total() const { return totalTimer.Ellapsed(); }
        double Delta() const { return lastDelta * timeScale * static_cast<float>(!paused); }
        double Unscaled() const { return lastDelta; }
        double Scale() const { return timeScale; }

        void SetPause(const bool InPause)
        {
            paused = InPause; 
        }
        
        bool IsPaused() const { return paused; }
        
    private:
        
        Timer totalTimer;
        
        double lastDelta = 0.0;
        double timeScale = 1.0f;
        bool paused = false; 
    };
}
