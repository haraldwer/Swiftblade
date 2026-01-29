#pragma once

#include "Utility/Collections/ConsumableQueue.h"

namespace Tasks
{
    class TaskBase
    {
        friend class Manager;
    public: 
        virtual ~TaskBase() = default;
        TaskBase() = default;
        
        virtual void Run() { CHECK_ASSERT(true, "Invalid task") }
        virtual Utility::TypeHash GetHash() { return Utility::TypeHash(); }
        
        Utility::Timepoint EnqueueTime() const { return enqueueTime; }
        Utility::Timepoint StartTime() const { return startTime; }
        Utility::Timepoint EndTime() const { return endTime; }
        
    private:
        Utility::Timepoint enqueueTime;
        Utility::Timepoint startTime;
        Utility::Timepoint endTime;
    };
    
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:
        void Init();
        void Deinit();
        
        void Enqueue(TaskBase* InTask);
        void Consume(const Utility::TypeHash& InHash, const std::function<bool(TaskBase*)>& InFunc, int InNum);

    private:
        bool run = false;
        Vector<std::thread> threads;
        std::mutex pendingMutex;
        Vector<TaskBase*> pendingTasks;
        
        std::mutex finishedMutex;
        Map<Utility::TypeHash, Utility::ConsumableQueue<TaskBase*>> finishedTasks;
        
        static constexpr int sleepDurationMS = 33;
        static constexpr int maxThreads = 8;
    };
}
