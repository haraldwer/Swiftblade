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
                front = (front + 1) % 2;
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
                    back = (back + 1) % 2;
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
        int front = 0;
        int back = 1;
        bool frontChanged = false;
        std::mutex lock;
    };
}
