#pragma once

namespace Utility
{
    template< typename T >
    typename std::vector<T>::iterator SortedInsert( std::vector<T> & InVec, T const& InItem )
    {
        return InVec.insert(
            std::ranges::upper_bound( InVec
                .begin(),
                InVec.end(),
                InItem ),
            InItem);
    }
    
    template< typename T, typename Pred >
    typename std::vector<T>::iterator SortedInsert( std::vector<T> & InVec, T const& InItem, Pred InPred )
    {
        return InVec.insert(
           std::ranges::upper_bound(
               InVec.begin(),
               InVec.end(),
               InItem,
               InPred ),
           InItem);
    }
}
