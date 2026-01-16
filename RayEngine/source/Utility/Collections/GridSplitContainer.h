#pragma once
#include "pch.h"
#include "Math/Coord.h"

namespace Utility
{
    template <class T, class KeyType = uint64, class CoordType = int16, int Depth = 3>
    class GridSplitContainer
    {
        
    public:
        using Coord = Coord<KeyType, CoordType>;
        static constexpr int depth = Depth; 
        
        class RootNode
        {
            friend class GridSplitContainer;
            RootNode(Coord InCoord) : coord(InCoord) {}
            Coord coord = Coord(0);
            Array<T, Math::StaticSumPow(8, Depth + 1)> nodes = {};
        };
        
        GridSplitContainer(float InCellSize = 1.0f)
        {
            cellSize = InCellSize;
        }
        
        void Expand(Vec3F InStart, Vec3F InEnd)
        {
            float size = GetDepthSize(0);
            Vec3I start = (InStart / size).To<int>(); // Round down
            Vec3I end = (InEnd / size + Vec3F::One()).To<int>(); // Round up
            for (int32 x = start.x; x <= end.x; ++x)
            for (int32 y = start.y; y <= end.y; ++y)
            for (int32 z = start.z; z <= end.z; ++z)
            {
                auto coord = Coord(x, y, z, 0);
                if (root.contains(coord.key))
                    continue;
                root.emplace(coord.key, RootNode(coord));
            }
        }
        
        void Clear()
        {
            root.clear();
        }
        
        template <class Func>
        void TraverseRoot(const Func& InFunc)
        {
            for (auto& node : root)
                InFunc(node.second);
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
        
        Vec3F GetRootPosition(const RootNode& InRoot) const
        {
            return Vec3F(InRoot.coord.pos.x, InRoot.coord.pos.y, InRoot.coord.pos.z) * GetDepthSize(0);
        }
        
        float GetDepthSize(const int InDepth) const
        {
            CHECK_ASSERT(InDepth > Depth, "Invalid depth, bigger than limit");
            CHECK_ASSERT(InDepth < 0, "Invalid depth, smaller than zero");
            return cellSize * static_cast<float>(1 << (Depth - InDepth - 1)); // Maybe /2
        }
    
    private:
        
        Map<KeyType, RootNode> root;
        float cellSize;
    };
}








































