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
            Vec3F position = {};
            float extent = {};
        };
        
        void Insert(const T& InData, const Cullable& InCull)
        {
            Insert(root, InData, InCull);
        }

        void Build()
        {
            Split(root, 0);
        }

        void Clear()
        {
            root = Node();
            nodes.clear();
        }

        // The query should return -1 if on left side of plane, 1 if on right side and 0 if on both sides.
        Vector<Vector<T>*> Get(const std::function<int(const Math::Plane<float>&)>& InQuery) const
        {
            Vector<Vector<T>*> result;
            Get(root, InQuery, result);
            return result;
        }

        const Vector<T>& GetAll() const
        {
            return root.data;
        }

        uint32 Count() const
        {
            return static_cast<uint32>(root.data.size());
        }

        bool Empty() const
        {
            return Count() == 0;
        }

    private:

        struct Node
        {
            Vec3F total = {};
            Vector<T> data = {}; // Memory alignment is important here!
            Vector<Cullable> cullData = {};
            Math::Plane<float> divisor = {};
            uint32 left = static_cast<uint32>(-1);
            uint32 right = static_cast<uint32>(-1);
        };
        
        static void Insert(Node& InNode, const T& InData, const Cullable& InCull)
        {
            InNode.data.push_back(InData);
            InNode.cullData.push_back(InCull);
            InNode.total += InCull.position;
        }
        
        void Split(Node& InNode, int InDepth)
        {
            CHECK_RETURN(InDepth == MaxDepth);
            CHECK_RETURN(InNode.cullData.size() < MinSplitCount);
            Vec3F average = InNode.total / static_cast<float>(InNode.cullData.size());

            // Find the max point
            Vec3F maxPoint;
            float maxDist = 0.0f;
            for (auto& data : InNode.cullData)
            {
                float sqrDist = (data.position - maxPoint).LengthSqr();
                if (sqrDist > maxDist)
                {
                    maxDist = sqrDist;
                    maxPoint = data.position;
                }
            }

            // Create plane
            Vec3F direction = (maxPoint - average).GetNormalized();
            InNode.divisor = Math::Plane(average, direction);

            // Create left and right nodes
            CHECK_ASSERT(InNode.left != static_cast<uint32>(-1), "Left index should not be set at this point");
            CHECK_ASSERT(InNode.right != static_cast<uint32>(-1), "Right index should not be set at this point");
            InNode.left = static_cast<uint32>(nodes.size());
            InNode.right = static_cast<uint32>(nodes.size() + 1);
            nodes.emplace_back();
            nodes.emplace_back();
            
            // Now every entry will be on either left, right or both sides of the plane
            for (int i = 0; i < static_cast<int>(InNode.cullData.size()); i++)
            {
                auto& data = InNode.data[i];
                auto& cullData = InNode.cullData[i];
                float distance = InNode.divisor.SignedDistance(cullData.position);
                if (abs(distance) < cullData.extent) // First, check face intersection
                {
                    Insert(nodes[InNode.left], data, cullData);
                    Insert(nodes[InNode.right], data,cullData);
                }
                else
                {
                    Insert(nodes[distance < 0 ? InNode.left : InNode.right], data, cullData);
                }
            }

            // Do not keep copies
            if (InDepth != 0)
            {
                InNode.data.clear();
                InNode.cullData.clear();
            }

            // Split both left and right
            Split(nodes[InNode.left], InDepth + 1);
            Split(nodes[InNode.right], InDepth + 1);
        }

        void Get(Node& InNode, const std::function<int(const Math::Plane<float>&)>& InQuery, Vector<Vector<T>*>& InOutResult) const
        {
            if (InNode.left == static_cast<uint32>(-1) && InNode.right == static_cast<uint32>(-1))
            {
                // Leaf node, return contents!
                if (!InNode.data.empty())
                    InOutResult.insert(&InNode.data);
                return;
            }
            
            // Get children
            CHECK_ASSERT(InNode.left == static_cast<uint32>(-1), "Invalid left index");
            CHECK_ASSERT(InNode.right == static_cast<uint32>(-1), "Invalid right index");

            const int result = InQuery(InNode.civisor);
            if (result <= 0)
                Get(nodes[InNode.left], InQuery, InOutResult);
            if (result >= 0)
                Get(nodes[InNode.right], InQuery, InOutResult);
        }
        
        Node root = {};
        Vector<Node> nodes = {};
     };
}
