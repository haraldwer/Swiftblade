#pragma once

namespace Utility
{
    // Thread safe buffering
    template <class T>
    class SwapBuffer
    {
    public:

        void SwapFront()
        {
            lock.lock();
            frontChanged = true;
            do
            {
                front = (front + 1) % 3;
            } while (front == back);
            lock.unlock();
        }
        
        const T& SwapBack()
        {
            lock.lock();
            if (frontChanged)
            {
                do
                {
                    back = (back + 1) % 3;
                } while (front == back);
                frontChanged = false;
            }
            lock.unlock();
            return Back();
        }
        
        T& Front() { return buffers[front]; }
        const T& Back() const { return buffers[back]; }
        Array<T, 3>& All() { return buffers; }
        
    private:
        Array<T, 3> buffers = {};
        std::mutex lock;
        size_t front = 0;
        size_t back = 1;
        bool frontChanged = false;
    };
}
