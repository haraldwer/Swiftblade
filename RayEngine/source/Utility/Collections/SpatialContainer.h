#pragma once
#include <functional>

#include "Utility/Math/Geometry/Plane.h"

namespace Utility
{
    template <class T, int MaxDepth = 5, int MinSplitCount = 10>
    class SpatialContainer
    {
    public:

        struct Cullable
        {
            Vec3F Position;
            float Extent;
        };
        
        void Insert(const T& InData, const Cullable& InCull)
        {
            Insert(Root, InData, InCull);
        }

        void Build()
        {
            Split(Root, 0);
        }

        void Clear()
        {
            Root = Node();
            Nodes.clear();
        }

        // The query should return -1 if on left side of plane, 1 if on right side and 0 if on both sides.
        Vector<Vector<T>*> Get(const std::function<int(const Math::Plane<float>&)>& InQuery) const
        {
            Vector<Vector<T>*> result;
            Get(Root, InQuery, result);
            return result;
        }

        const Vector<T>& GetAll() const
        {
            return Root.Data;
        }

        uint32 Count() const
        {
            return static_cast<uint32>(Root.Data.size());
        }

        bool Empty() const
        {
            return Count() == 0;
        }

    private:

        struct Node
        {
            Vec3F Total;
            Vector<T> Data; // Memory alignment is important here!
            Vector<Cullable> CullData;
            Math::Plane<float> Divisor;
            uint32 Left = static_cast<uint32>(-1);
            uint32 Right = static_cast<uint32>(-1);
        };
        
        static void Insert(Node& InNode, const T& InData, const Cullable& InCull)
        {
            InNode.Data.push_back(InData);
            InNode.CullData.push_back(InCull);
            InNode.Total += InCull.Position;
        }
        
        void Split(Node& InNode, int InDepth)
        {
            CHECK_RETURN(InDepth == MaxDepth);
            CHECK_RETURN(InNode.CullData.size() < MinSplitCount);
            Vec3F average = InNode.Total / static_cast<float>(InNode.CullData.size());

            // Find the max point
            Vec3F maxPoint;
            float maxDist = 0.0f;
            for (auto& data : InNode.CullData)
            {
                float sqrDist = (data.Position - maxPoint).LengthSqr();
                if (sqrDist > maxDist)
                {
                    maxDist = sqrDist;
                    maxPoint = data.Position;
                }
            }

            // Create plane
            Vec3F direction = (maxPoint - average).GetNormalized();
            InNode.Divisor = Math::Plane(average, direction);

            // Create left and right nodes
            CHECK_ASSERT(InNode.Left != static_cast<uint32>(-1), "Left index should not be set at this point");
            CHECK_ASSERT(InNode.Right != static_cast<uint32>(-1), "Right index should not be set at this point");
            InNode.Left = static_cast<uint32>(Nodes.size());
            InNode.Right = static_cast<uint32>(Nodes.size() + 1);
            Nodes.emplace_back();
            Nodes.emplace_back();
            
            // Now every entry will be on either left, right or both sides of the plane
            for (int i = 0; i < static_cast<int>(InNode.CullData.size()); i++)
            {
                auto& data = InNode.Data[i];
                auto& cullData = InNode.CullData[i];
                float distance = InNode.Divisor.SignedDistance(cullData.Position);
                if (abs(distance) < cullData.Extent) // First, check face intersection
                {
                    Insert(Nodes[InNode.Left], data, cullData);
                    Insert(Nodes[InNode.Right], data,cullData);
                }
                else
                {
                    Insert(Nodes[distance < 0 ? InNode.Left : InNode.Right], data, cullData);
                }
            }

            // Do not keep copies
            if (InDepth != 0)
            {
                InNode.Data.clear();
                InNode.CullData.clear();
            }

            // Split both left and right
            Split(Nodes[InNode.Left], InDepth + 1);
            Split(Nodes[InNode.Right], InDepth + 1);
        }

        void Get(Node& InNode, const std::function<int(const Math::Plane<float>&)>& InQuery, Vector<Vector<T>*>& InOutResult) const
        {
            if (InNode.Left == static_cast<uint32>(-1) && InNode.Right == static_cast<uint32>(-1))
            {
                // Leaf node, return contents!
                if (!InNode.Data.empty())
                    InOutResult.insert(&InNode.Data);
                return;
            }
            
            // Get children
            CHECK_ASSERT(InNode.Left == static_cast<uint32>(-1), "Invalid left index");
            CHECK_ASSERT(InNode.Right == static_cast<uint32>(-1), "Invalid right index");

            int result = InQuery(InNode.Divisor);
            if (result <= 0)
                Get(Nodes[InNode.Left], InQuery, InOutResult);
            if (result >= 0)
                Get(Nodes[InNode.Right], InQuery, InOutResult);
        }
        
        Node Root;
        Vector<Node> Nodes;
     };
}
