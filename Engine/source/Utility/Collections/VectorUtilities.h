#pragma once

namespace Utility
{
    // Merge a vector of vectors
    template <class T>
    Vector<T> MergeVectors(Vector<const Vector<T>*> InVectors)
    {
        Vector<T> merge;
        int c = 0;
        for (auto& vec : InVectors)
            c += vec->size();
        merge.reserve(c);
        for (auto& vec : InVectors)
            merge.insert(merge.end(), vec->begin(), vec->end());
        return merge;
    }
    
    struct Group
    {
        uint32 start = 0;
        uint32 end = 0;
    };
    
    // Create groups from sorted indices
    inline Vector<Group> GroupIndices(const Set<uint32>& InIndices)
    {
        Vector<Group> groups;
        uint32 start = 0;
        uint32 end = static_cast<uint32>(-1);
        for (uint32 i : InIndices)
        {
            if (i != end + 1 && end != static_cast<uint32>(-1))
            {
                groups.emplace_back(start, end);
                start = i;
            }
            end = i;
        }
        if (start != end && end != static_cast<uint32>(-1))
            groups.emplace_back(start, end);
        return groups;
    }

    // Group data from sorted indices
    template <class T>
    Vector<T> GroupData(const Vector<T>& InData, const Set<uint32>& InIndices)
    {
        Vector<T> result;
        uint32 start = 0;
        uint32 end = static_cast<uint32>(-1);
        for (uint32 i : InIndices)
        {
            if (i != end + 1 && end != static_cast<uint32>(-1))
            {
                result.insert(result.end(), InData.begin() + start, InData.begin() + end + 1);
                start = i;
            }
            end = i;
        }
        if (start != end && end != static_cast<uint32>(-1))
            result.insert(result.end(), InData.begin() + start, InData.begin() + end + 1);
        return result;
    }
}
