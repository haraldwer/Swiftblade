#pragma once
#include <functional>

#include "Cullable.h"
#include "Utility/Math/Geometry/Plane.h"

namespace Utility
{
    template <class T, int MaxDepth = 5, int MinSplitCount = 50, int NormalSampleCount = 20>
    class SplitContainer
    {
    public:
        
        void Insert(const T& InData, const Cullable& InCull)
        {
            CHECK_ASSERT(nodes.empty(), "No root node");
            Insert(nodes[0], InData, InCull);
        }

        void Build()
        {
            CHECK_ASSERT(nodes.empty(), "No root node");
            Split(0, 0);
        }

        void Clear()
        {
            size_t s = nodes.size();
            nodes.clear();
            nodes.reserve(s);
            nodes.emplace_back(); // Root
        }
        
        Vector<const Vector<T>*> Get(const Vector<Vec3F>& InPoints) const
        {
            CHECK_ASSERT(nodes.empty(), "No root node");
            Vector<const Vector<T>*> result;
            Get(nodes.at(0), InPoints, result);
            return result;
        }

        struct DebugNode
        {
            Math::Plane<float> divisor = {};
            bool leaf = false;
            bool inside = false;
            int depth;
        };
        
        Vector<DebugNode> GetDebug(const Vector<Vec3F>& InPoints) const
        {
            CHECK_ASSERT(nodes.empty(), "No root node");
            Vector<DebugNode> result;
            GetDebug(nodes.at(0), 0, InPoints, result);
            return result;
        }
        
        const Vector<T>& GetAll() const
        {
            CHECK_ASSERT(nodes.empty(), "No root node");
            return nodes.at(0).data;
        }

        uint32 Count() const
        {
            CHECK_ASSERT(nodes.empty(), "No root node");
            return static_cast<uint32>(nodes.at(0).data.size());
        }

        bool Empty() const
        {
            return Count() == 0;
        }

        bool HasSplit() const
        {
            return nodes.size() > 1;
        }

    private:

        struct Node
        {
            Vec3F total = {};
            float maxExtent = 0.0f;
            Vector<T> data = {}; // Memory alignment is important here!
            Vector<Cullable> cullData = {};
            Math::Plane<float> divisor = {};
            uint32 left = static_cast<uint32>(-1);
            uint32 right = static_cast<uint32>(-1);
        };

        enum class SpatialQueryResult : uint8
        {
            NONE_INSIDE = 0,
            LEFT_INSIDE,
            RIGHT_INSIDE,
            BOTH_INSIDE,
        };
        
        static void Insert(Node& InNode, const T& InData, const Cullable& InCull)
        {
            InNode.data.push_back(InData);
            InNode.cullData.push_back(InCull);
            InNode.total += InCull.position;
            InNode.maxExtent = Math::Max(InNode.maxExtent, InCull.extent);
        }
        
        void Split(uint32 InNodeIndex, int InDepth)
        {
            CHECK_ASSERT(InNodeIndex >= nodes.size(), "Invalid node index");
            auto& earlyNode = nodes[InNodeIndex];
            CHECK_RETURN(InDepth == MaxDepth);
            CHECK_RETURN(earlyNode.cullData.size() < MinSplitCount);

            Vec3F average = earlyNode.total / static_cast<float>(earlyNode.cullData.size());

            // Find the max point (randomly select a few points)
            Vec3F maxPoint;
            float maxDist = 0.0f;
            int c = Math::Min(static_cast<int>(earlyNode.data.size()), NormalSampleCount);
            float step = c / static_cast<float>(earlyNode.data.size());
            for (int i = 0; i < c; i++)
            {
                int index = static_cast<int>(step * i);
                auto& data = earlyNode.cullData[index];
                float sqrDist = (data.position - maxPoint).LengthSqr();
                if (sqrDist > maxDist)
                {
                    maxDist = sqrDist;
                    maxPoint = data.position;
                }
            }

            // Create plane
            Vec3F direction = (maxPoint - average).GetNormalized();
            earlyNode.divisor = Math::Plane(average, direction);

            // Create left and right nodes
            CHECK_ASSERT(earlyNode.left != static_cast<uint32>(-1), "Left index should not be set at this point");
            CHECK_ASSERT(earlyNode.right != static_cast<uint32>(-1), "Right index should not be set at this point");

            uint32 left = static_cast<uint32>(nodes.size());
            uint32 right = static_cast<uint32>(nodes.size() + 1);
            earlyNode.left = left;
            earlyNode.right = right;
            nodes.emplace_back();
            nodes.emplace_back();

            // Cache a new ref after changing nodes vector
            auto& lateNode = nodes[InNodeIndex];
            size_t expectedSize = lateNode.data.size() / 2;
            auto& leftNode = nodes[left];
            auto& rightNode = nodes[right];
            leftNode.data.reserve(expectedSize);
            leftNode.cullData.reserve(expectedSize);
            rightNode.data.reserve(expectedSize);
            rightNode.cullData.reserve(expectedSize);
            
            // Now every entry will be on either left, right or both sides of the plane
            for (int i = 0; i < static_cast<int>(lateNode.cullData.size()); i++)
            {
                auto& data = lateNode.data[i];
                auto& cullData = lateNode.cullData[i];
                float distance = lateNode.divisor.SignedDistance(cullData.position);
                Insert(distance < 0 ? leftNode : rightNode, data, cullData);
            }

            // Do not keep copies
            if (InDepth != 0)
            {
                lateNode.data.clear();
                lateNode.cullData.clear();
            }

            // Split both left and right
            Split(left, InDepth + 1);
            Split(right, InDepth + 1);
        }

        void Get(const Node& InNode, const Vector<Vec3F>& InPoints, Vector<const Vector<T>*>& InOutResult) const
        {
            if (InNode.left == static_cast<uint32>(-1) && InNode.right == static_cast<uint32>(-1))
            {
                // Leaf node, return contents!
                if (!InNode.data.empty())
                    InOutResult.push_back(&InNode.data);
                return;
            }
            
            // Get children
            CHECK_ASSERT(InNode.left == static_cast<uint32>(-1), "Invalid left index");
            CHECK_ASSERT(InNode.right == static_cast<uint32>(-1), "Invalid right index");

            switch (Query(InNode, InPoints))
            {
            case SpatialQueryResult::LEFT_INSIDE:
                Get(nodes.at(InNode.left), InPoints, InOutResult);
                break;
            case SpatialQueryResult::RIGHT_INSIDE:
                Get(nodes.at(InNode.right), InPoints, InOutResult);
                break;
            case SpatialQueryResult::BOTH_INSIDE:
                Get(nodes.at(InNode.left), InPoints, InOutResult);
                Get(nodes.at(InNode.right), InPoints, InOutResult);
                break;
            default:
            case SpatialQueryResult::NONE_INSIDE:
                CHECK_ASSERT(true, "At least one side has to be partially inside the shape")
                break;
            }
        }

        void GetDebug(const Node& InNode, int InDepth, const Vector<Vec3F>& InPoints, Vector<DebugNode>& InOutResult) const
        {
            auto& debugNode = InOutResult.emplace_back();
            debugNode.divisor = InNode.divisor;
            debugNode.inside = true;
            debugNode.depth = InDepth;
            
            if (InNode.left == static_cast<uint32>(-1) && InNode.right == static_cast<uint32>(-1))
            {
                debugNode.leaf = true;
                return;
            }
            
            // Get children
            CHECK_ASSERT(InNode.left == static_cast<uint32>(-1), "Invalid left index");
            CHECK_ASSERT(InNode.right == static_cast<uint32>(-1), "Invalid right index");

            switch (Query(InNode, InPoints))
            {
            case SpatialQueryResult::LEFT_INSIDE:
                {
                    GetDebug(nodes.at(InNode.left), InDepth + 1, InPoints, InOutResult);
                    auto& rightNode = InOutResult.emplace_back();
                    rightNode.divisor = nodes.at(InNode.right).divisor;
                    rightNode.depth = InDepth + 1;
                    break;
                }
            case SpatialQueryResult::RIGHT_INSIDE:
                {
                    GetDebug(nodes.at(InNode.right), InDepth + 1, InPoints, InOutResult);
                    auto& leftNode = InOutResult.emplace_back();
                    leftNode.divisor = nodes.at(InNode.left).divisor;
                    leftNode.depth = InDepth + 1;
                    break;
                }
            case SpatialQueryResult::BOTH_INSIDE:
                GetDebug(nodes.at(InNode.left), InDepth + 1, InPoints, InOutResult);
                GetDebug(nodes.at(InNode.right), InDepth + 1, InPoints, InOutResult);
                break;
            default:
            case SpatialQueryResult::NONE_INSIDE:
                CHECK_ASSERT(true, "At least one side has to be partially inside the shape")
                break;
            }
        }

        static SpatialQueryResult Query(const Node& InNode, const Vector<Vec3F>& InPoints)
        {
            int inside = 0;
            for (auto& point : InPoints)
                inside += InNode.divisor.IsInside(point);
            if (inside == 0)
                return SpatialQueryResult::RIGHT_INSIDE;
            if (inside == static_cast<int>(InPoints.size()))
                return SpatialQueryResult::LEFT_INSIDE;
            return SpatialQueryResult::BOTH_INSIDE;
        }
        
        Vector<Node> nodes = { Node() };
     };
}
