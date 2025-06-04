#pragma once
#include "Utility/Math/Random.hpp"

namespace Utility
{
    template <class T, class WeightT = float>
    class RandomWeightedCollection
    {
    public:
        RandomWeightedCollection(const int InSeed) : rand(InSeed) {}

        void Add(const T& InEntry, const WeightT InWeight = 1.0f)
        {
            entries.push_back({ InEntry, InWeight });
        }
        
        void Clear()
        {
            entries.clear(); 
            indices.clear();
        }
        
        void Reset()
        {
            indices.clear();
        }

        T Pop()
        {
            if (entries.empty())
                return T(); 
            
            if (indices.empty())
            {
                for (size_t i = 0; i < entries.size(); i++)
                    indices.insert(static_cast<int>(i));
                total = 0.0;
                for (const Entry& entry : entries)
                    total += entry.weight;
            }

            // Random range
            WeightT val = rand.Range<WeightT>(0.0, total); 
            
            // Subtract until zero
            for (int index : indices)
            {
                auto& entry = entries[index];
                val -= entry.weight;
                if (val < 0.0)
                {
                    // Subtract from total and return result
                    total -= entry.weight;
                    indices.erase(index); 
                    return entry.data; 
                }
            }
            
            return T();
        }

        int Count()
        {
            return static_cast<int>(entries.size());
        }

    private:

        struct Entry
        {
            T data = {};
            WeightT weight = {};
        };
        
        Vector<Entry> entries = {};
        Set<int> indices = {};
        WeightT total = 0.0;
        Random rand = {}; 
    };
}
