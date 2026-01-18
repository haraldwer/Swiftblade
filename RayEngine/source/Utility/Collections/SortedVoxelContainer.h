#pragma once

#include "Math/Coord.h"

/*

namespace Utility
{
    template <class T, class KeyType = uint64, class CoordType = int16>
    class SortedVoxelContainer
    {
    public:
        using Coord = Coord<KeyType, CoordType>; 
        
        SortedVoxelContainer(int InSize)
        {
            CHECK_ASSERT(InSize != Math::NextPow2(InSize), "Invalid size");
            
            size = InSize;
            int layerSize = size;
            while (true)
            {
                depth++;
                int layerEntries = layerSize ^ 3;
                layers.emplace_back().reserve(layerEntries);
                layerSize /= 2;
                if (layerSize < 1)
                    break;
            }
        }
        
        template <class Func>
        static void GetNode(RootNode& InRoot, const int InIndex, const int InDepth, const Func& InFunc)
        {
            // Index är "CurrentLayerIndex" där layerStart går att räkna ut
            const int layerStartIndex = Math::StaticSumPow(8, InDepth);
            const int index = layerStartIndex + InIndex;
            InFunc(InRoot.nodes.at(index));
        }

        template <class F>
        static void TraverseChildren(RootNode& InRoot, const int InIndex, const int InDepth, const F& InFunc)
        {
            const int nextLayerStart = Math::StaticSumPow(8, InDepth + 1);
            const int nextLayerStartIndex = nextLayerStart + InIndex * 8;
            CHECK_ASSERT(nextLayerStartIndex + 7 > InRoot.nodes.size(), "Invalid index");
            for (int i = 0; i < 8; i++)
                InFunc(nextLayerStartIndex + i);
        }

        Vec3F GetNodeOffset(const int InIndex, const int InDepth) const
        {
            const int side = InIndex % 8;
            return Vec3F(
                side & 4 ? 1.0f : -1.0f,
                side & 2 ? 1.0f : -1.0f,
                side & 1 ? 1.0f : -1.0f
            ) * GetDepthSize(InDepth);
        }
        
    private:
        
        struct Layer
        {
            int lowestIndex = -1;
            Vector<T> entries;
        };
        
        std::function<T> compare;
        Vector<Layer> layers;
        int size = 0;
        int depth = 0;
    };
}

*/