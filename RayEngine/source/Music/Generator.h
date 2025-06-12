#pragma once
#include "Collections/SwapBuffer.h"

namespace Audio
{
    struct Config : PropertyOwner<Config>
    {
        
    };

    struct Feedback
    {
        
    };

    class Generator
    {
    public:
        ~Generator();
        void Init();
        void Deinit();
        
        Utility::SwapBuffer<Config> config; // Game -> Music
        Utility::SwapBuffer<Feedback> feedback; // Music -> Game
        
    private:
        void ThreadWork();
        void Tick();
        
        std::thread audioThread;
        bool run = false;
    };
}
