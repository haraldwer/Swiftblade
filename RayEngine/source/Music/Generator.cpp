#include "Generator.h"

Audio::Generator::~Generator()
{
    if (audioThread.joinable())
        audioThread.join();
}

void Audio::Generator::Init()
{
    run = true;
    audioThread = std::thread([&]()
    {
        ThreadWork();
    });
}

void Audio::Generator::Deinit()
{
    run = false;
}

void Audio::Generator::ThreadWork()
{
    PROFILE_AU();
    
    // Create resources

    // Update
    while (run)
        Tick();
    
    // Free resources
}

void Audio::Generator::Tick()
{
    PROFILE_AU();
    
    // Working config...
    auto& conf = config.SwapBack();


    
    
    feedback.SwapFront();

    std::this_thread::yield();
}
