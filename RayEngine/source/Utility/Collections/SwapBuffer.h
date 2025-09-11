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
#ifndef __EMSCRIPTEN__
            lock.lock();
#endif
            frontChanged = true;
            do
            {
                front = (front + 1) % 3;
            } while (front == back);
#ifndef __EMSCRIPTEN__
            lock.unlock();
#endif
        }
        
        const T& SwapBack()
        {
#ifndef __EMSCRIPTEN__
            lock.lock();
#endif
            if (frontChanged)
            {
                do
                {
                    back = (back + 1) % 3;
                } while (front == back);
                frontChanged = false;
            }
#ifndef __EMSCRIPTEN__
            lock.unlock();
#endif
            return Back();
        }
        
        T& Front() { return buffers[front]; }
        const T& Back() const { return buffers[back]; }
        Array<T, 3>& All() { return buffers; }
        
    private:
        Array<T, 3> buffers = {};
#ifndef __EMSCRIPTEN__
        std::mutex lock;
#endif
        size_t front = 0;
        size_t back = 1;
        bool frontChanged = false;
    };
}
