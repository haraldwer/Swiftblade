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
        
        T& Curr() { return buffers.at(current); } 
        const T& Curr() const { return buffers.at(current); } 
        T& Prev(const int InOffset = 1) { return buffers.at((current + buffers.size() - InOffset) % buffers.size()); }
        const T& Prev(const int InOffset = 1) const { return buffers.at((current + buffers.size() - InOffset) % buffers.size()); }
        void Iterate() { current = (current + 1) % static_cast<int>(buffers.size()); }
        Vector<T>& All() { return buffers; }
        const Vector<T>& All() const { return buffers; }
        void Reset() { current = 0; }

    private:
        Vector<T> buffers = {};
        int current = 0;
    };
}
