#pragma once
#include "Utility/Math/Random.hpp"

namespace Utility
{
    template <class T, class WeightT = float>
    class RandomWeightedCollection
    {
    public:
        RandomWeightedCollection(const int InSeed) : Rand(InSeed) {}

        void Add(const T& InEntry, const WeightT InWeight = 1.0f)
        {
            Entries.push_back({ InEntry, InWeight });
        }
        
        void Clear()
        {
            Entries.clear(); 
            Indices.clear();
        }
        
        void Reset()
        {
            Indices.clear();
        }

        T Pop()
        {
            if (Entries.empty())
                return T(); 
            
            if (Indices.empty())
            {
                for (size_t i = 0; i < Entries.size(); i++)
                    Indices.insert(static_cast<int>(i));
                Total = 0.0;
                for (const Entry& entry : Entries)
                    Total += entry.Weight;
            }

            // Random range
            WeightT val = Rand.Range<WeightT>(0.0, Total); 
            
            // Subtract until zero
            for (int index : Indices)
            {
                auto& entry = Entries[index];
                val -= entry.Weight;
                if (val < 0.0)
                {
                    // Subtract from total and return result
                    Total -= entry.Weight;
                    Indices.erase(index); 
                    return entry.Data; 
                }
            }
            
            return T();
        }

        int Count()
        {
            return static_cast<int>(Entries.size());
        }

    private:

        struct Entry
        {
            T Data = {};
            WeightT Weight = {};
        };
        
        Vector<Entry> Entries = {};
        Set<int> Indices = {};
        WeightT Total = 0.0;
        Random Rand = {}; 
    };
}
