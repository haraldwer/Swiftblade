#include "Manager.h"

void Tasks::Manager::Init()
{
    PROFILE();
    
    CHECK_ASSERT(!threads.empty(), "Threads already started");
    
    int hardwareThreads = static_cast<int>(std::thread::hardware_concurrency()); 
    int numThreadsMax = std::min(maxThreads, hardwareThreads - 2);
    int numThreads = std::max(1, numThreadsMax);
    LOG("Starting threads: ", numThreads)
    
    threads.resize(numThreads);
    pendingMutex.lock();
    finishedMutex.lock();
    run = true;
    for (int i = 0; i < numThreads; i++)
    {
        threads.at(i) = std::thread([&]
        {
            while (run)
            {
                if (!pendingMutex.try_lock())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepDurationMS));
                    continue;
                }
                
                if (pendingTasks.empty())
                {
                    pendingMutex.unlock();
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepDurationMS));
                    continue;
                }

                auto task = pendingTasks.front();
                pendingTasks.erase(pendingTasks.begin());
                pendingMutex.unlock();
                CHECK_CONTINUE(!task);
                
                // Do work
                task->startTime = Utility::Now();
                task->Run();
                task->endTime = Utility::Now();
                
                // TODO: Print result

                // Add to queue
                auto hash = task->GetHash();
                finishedMutex.lock();
                auto& finishedQueue = finishedTasks[hash];
                finishedMutex.unlock();
                finishedQueue.Enqueue(task);
                
                std::this_thread::yield();
            }
        });
    }
    pendingMutex.unlock();
    finishedMutex.unlock();
}

void Tasks::Manager::Deinit()
{
    PROFILE(); 
    
    pendingMutex.lock();
    pendingTasks.clear();
    pendingMutex.unlock();
    
    run = false;
    for (auto& t : threads)
        if (t.joinable())
            t.join();
    threads.clear();
    
    finishedMutex.lock();
    finishedTasks.clear();
    finishedMutex.unlock();
}

void Tasks::Manager::Enqueue(TaskBase* InTask)
{
    PROFILE();
    CHECK_ASSERT(!InTask, "Invalid task");
    pendingMutex.lock();
    pendingTasks.push_back(InTask);
    InTask->enqueueTime = Utility::Now();
    pendingMutex.unlock();
}

void Tasks::Manager::Consume(const Utility::TypeHash &InHash, const std::function<bool(TaskBase *)> &InFunc, int InNum) 
{
    PROFILE();
    finishedMutex.lock();
    auto& queue = finishedTasks[InHash];
    finishedMutex.unlock();
    queue.TryConsume(InFunc, InNum);
}

