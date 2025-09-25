#pragma once

namespace Utility
{
    template <class T>
    class OrderedBuffer
    {
    public:
        OrderedBuffer(const int InNum = 2)
        {
            CHECK_ASSERT(InNum < 2, "Too few targets");
            buffers.resize(InNum);
        }
        
        T& Curr() { return buffers[current]; } 
        T& Prev(const int InOffset = 1) { return buffers[(current + buffers.size() - InOffset) % buffers.size()]; }
        void Iterate() { current = (current + 1) % static_cast<int>(buffers.size()); }
        Vector<T>& All() { return buffers; }
        void Reset() { current = 0; }

    private:
        Vector<T> buffers = {};
        int current = 0;
    };
}
