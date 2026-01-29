#pragma once
#include <thread>

namespace Rendering
{
    struct PaceConfig : PropertyOwner<PaceConfig>
    {
        
    };
    
    class FramePacer : public Utility::Singelton<FramePacer, true>
    {
    public:
        ~FramePacer() override = default;
        FramePacer() = default;
        FramePacer(
            const int InTargetFramerate, 
            const int InTargetTickrate, 
            const double InMaxFrameTickTime, 
            const double InSleepMargin, 
            const double InMinDelta, 
            const double InMaxDelta) : 
            targetFramerate(InTargetFramerate), 
            targetTickrate(InTargetTickrate),
            maxFrameTickTime(InMaxFrameTickTime),
            sleepMargin(InSleepMargin),
            minDelta(InMinDelta),
            maxDelta(InMaxDelta) { }
        
        int Pace()
        {
            RN_PROFILE();
            frameDelta = CapFPS();
            return CalculateTicks(frameDelta);
        }
        
        double TickDelta() const
        {
            return Utility::Math::Clamp(1.0 / targetTickrate, minDelta, maxDelta);
        }
        double FrameDelta() const
        {
            return Utility::Math::Clamp(frameDelta, minDelta, maxDelta);
        }
        
    private:
        
        double CapFPS()
        {
            RN_PROFILE();
            double ellapsed = 0.0;
            double delta = 0.0;
            
            while (true)
            {
                // Calculate delta
                ellapsed = timer.Ellapsed();
                delta = ellapsed - timestamp;
                if (targetFramerate <= 0) // No cap
                    break;
                
                const double desiredFrameDelta = 1.0 / targetFramerate;
                const double totalDelta = delta + frameLeftoverDelta;
                
                if (totalDelta >= desiredFrameDelta)
                {
                    frameLeftoverDelta = totalDelta - desiredFrameDelta;
                    break;
                }
                
                const double remaining = desiredFrameDelta - totalDelta;
                if (remaining > sleepMargin)
                    std::this_thread::sleep_for(std::chrono::duration<double>(remaining - sleepMargin));
                else
                    std::this_thread::yield();
            }
            
            timestamp = ellapsed;
            while (timestamp > 180.0f) // Precision reset
            {
                timestamp -= 180.0f;
                timer = Utility::Timer();
            }
            
            return delta;
        }
        
        int CalculateTicks(const double InDelta)
        {
            RN_PROFILE();
            if (targetTickrate <= 0)
                return 1;
            
            // What's the desired tickrate?
            // How many ticks passed?
            const double tickDelta = 1.0 / targetTickrate;
            tickTimeCounter = Utility::Math::Min(tickTimeCounter + InDelta, maxFrameTickTime);
            int ticks = 0;
            while (tickTimeCounter >= tickDelta)
            {
                tickTimeCounter -= tickDelta;
                ticks++;
            }
            return ticks;
        }
        
        Utility::Timer timer;
        double timestamp = 0.0;
        double tickTimeCounter = 0.0;
        double frameLeftoverDelta = 0.0;
        double frameDelta = 0;
        
        int targetFramerate = 300;
        int targetTickrate = 300;
        double maxFrameTickTime = 0.0333;
        double sleepMargin = 0.001;
        double minDelta = 1.0 / 2000;
        double maxDelta = 1.0;
    };
}
