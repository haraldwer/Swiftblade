#pragma once

namespace Utility
{
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
}
