#pragma once
#include "Manager.h"

namespace Tasks
{
    template<typename TInData, typename TOutData>
    struct Task : TaskBase
    {
        Task(const TInData& InData, const std::function<TOutData(const TInData&)>& InTask) : in(InData), task(InTask) {}
        Utility::TypeHash GetHash() override { return Utility::Type::GetHash<Task>(); }
        
        void Run() override
        {
            PROFILE();
            out = task(in);
        }
        
        TInData in;
        TOutData out;
        std::function<TOutData(const TInData&)> task;
    };
    
    template<typename TInData, typename TOutData>
    void Enqueue(const TInData& InData, const std::function<TOutData(const TInData&)>& InTask)
    {
        PROFILE();
        auto task = new Task(InData, InTask);
        Manager::Get().Enqueue(reinterpret_cast<TaskBase*>(task));
    }
    
    template<typename TInData, typename TOutData>
    void Consume(const std::function<bool(const TInData&, const TOutData&)>& InConsumeFunc, int InNum = 0)
    {
        PROFILE();
        Manager::Get().Consume(Utility::Type::GetHash<Task<TInData, TOutData>>(), [consumeFunc = InConsumeFunc](TaskBase* InTask)
        {
            auto task = reinterpret_cast<Task<TInData, TOutData>*>(InTask);
            if (consumeFunc(task->in, task->out))
            {
                delete task;
                return true;
            }
            return false;
        }, InNum);
    }
}