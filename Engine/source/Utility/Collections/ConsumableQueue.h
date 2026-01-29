#pragma once

namespace Utility
{
    // Thread-safe consumable queue
    template<class T>
    class ConsumableQueue
    {
    public:
        void Enqueue(const T& InValue)
        {
            mutex.lock();
            queue.push_back(InValue);
            mutex.unlock();
        }
        
        void Consume(const std::function<bool(const T&)>& InConsume, const int InNum = 0)
        {
            mutex.lock();
            ConsumeInternal(InConsume, InNum);
            mutex.unlock();
        }
    
        void TryConsume(const std::function<bool(const T&)>& InConsume, const int InNum = 0)
        {
            if (mutex.try_lock())
            {
                ConsumeInternal(InConsume, InNum);
                mutex.unlock();
            }
        }
        
        void Clear()
        {
            mutex.lock();
            queue.clear();
            mutex.unlock();
        }
        
    private:
        
        void ConsumeInternal(const std::function<bool(const T&)>& InConsume, const int InNum = 0)
        {
            int consumed = 0;
            for (int i = queue.size() - 1; i >= 0; --i)
            {
                if (InConsume(queue.at(i)))
                {
                    queue.erase(queue.begin() + i);
                    consumed++;
                    if (consumed >= InNum && InNum > 0)
                        break;
                }
            }
        }
        
        Vector<T> queue;
        std::mutex mutex;
    };
}