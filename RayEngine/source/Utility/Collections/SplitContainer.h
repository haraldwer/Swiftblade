#pragma once

#include "Cullable.h"
#include "VectorUtilities.h"
#include "Utility/Math/Geometry/Plane.h"

namespace Utility
{
    template <class T, int MaxDepth = 5, int MaxDist = 5, int MinSplitCount = 50, int NormalSampleCount = 20>
    class SplitContainer
    {
    public:
        
        void Insert(const T& InData, const Cullable& InCull)
        {
            CHECK_ASSERT(data.size() != cullData.size(), "Invalid size");
            data.push_back(InData);
            cullData.push_back(InCull);
        }

        void Build()
        {
            CHECK_ASSERT(data.size() != cullData.size(), "Invalid size");
            size_t s = nodes.size();
            nodes.clear();
            nodes.reserve(s);
            auto& node = nodes.emplace_back();
            for (int i = 0; i < data.size(); i++)
                Insert(node, i);
            Split(0, 0);

            for (auto& n : nodes)
                for (auto& index : n.indices)
                    n.cache.push_back(data[index]);
        }

        void Clear()
        {
            size_t c = data.size();
            data.clear();
            cullData.clear();
            data.reserve(c);
            cullData.reserve(c);
            size_t s = nodes.size();
            nodes.clear();
            nodes.reserve(s);
        }
        
        Set<uint32> GetIndices(const Vector<Vec3F>& InPoints) const
        {
            CHECK_RETURN(nodes.empty(), {});
            Set<uint32> indices;
            Set<uint32> nodeIndices;
            Get(0, InPoints, nodeIndices);
            for (auto& nI : nodeIndices)
            {
                auto& node = nodes[nI];
                indices.insert(node.indices.begin(), node.indices.end());
            }
            return indices;
        }

        Vector<T> GetCulled(const Vector<Vec3F>& InPoints) const
        {
            Vector<T> result;
            Set<uint32> nodeIndices;
            Get(0, InPoints, nodeIndices);
            for (auto& nI : nodeIndices)
            {
                auto& node = nodes[nI];
                result.insert(result.end(), node.cache.begin(), node.cache.end());
            }
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
            CHECK_RETURN(nodes.empty(), {});
            Vector<DebugNode> result;
            GetDebug(nodes.at(0), 0, InPoints, result);
            return result;
        }
        
        const Vector<T>& GetAll() const
        {
            return data;
        }

        uint32 Count() const
        {
            return static_cast<uint32>(data.size());
        }

        bool Empty() const
        {
            return Count() == 0;
        }

    private:

        Vector<T> data;
        Vector<Cullable> cullData;
        
        struct Node
        {
            Vector<T> cache;
            Vector<uint32> indices = {};
            Vec3F total = {};
            float maxExtent = 0.0f;
            Math::Plane<float> divisor = {};
            uint32 left = static_cast<uint32>(-1);
            uint32 right = static_cast<uint32>(-1);
        };

        Vector<Node> nodes = { Node() };

        enum class SpatialQueryResult : uint8
        {
            NONE_INSIDE = 0,
            LEFT_INSIDE,
            RIGHT_INSIDE,
            BOTH_INSIDE,
        };
        
        void Insert(Node& InNode, uint32 InIndex)
        {
            InNode.indices.push_back(InIndex);
            auto& cull = cullData.at(InIndex);
            InNode.total += cull.position;
            InNode.maxExtent = Math::Max(InNode.maxExtent, cull.extent);
        }
        
        void Split(uint32 InNodeIndex, int InDepth)
        {
            CHECK_ASSERT(InNodeIndex >= nodes.size(), "Invalid node index");
            auto& earlyNode = nodes[InNodeIndex];
            size_t size = earlyNode.indices.size();
            CHECK_RETURN(InDepth == MaxDepth);
            CHECK_RETURN(size < MinSplitCount);

            Vec3F average = earlyNode.total / static_cast<float>(size);

            // Find the max point (randomly select a few points)
            Vec3F maxPoint;
            float maxDist = 0.0f;
            int c = Math::Min(static_cast<int>(size), NormalSampleCount);
            float step = static_cast<float>(c) / static_cast<float>(size);
            for (int i = 0; i < c; i++)
            {
                int stepIndex = static_cast<int>(step * static_cast<float>(i));
                int dataIndex = earlyNode.indices.at(stepIndex);
                auto& cull = cullData.at(dataIndex);
                float sqrDist = (cull.position - maxPoint).LengthSqr();
                if (sqrDist > maxDist)
                {
                    maxDist = sqrDist;
                    maxPoint = cull.position;
                }
            }

            CHECK_RETURN(maxDist < MaxDist);

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
            size_t expectedSize = lateNode.indices.size() / 2;
            auto& leftNode = nodes[left];
            auto& rightNode = nodes[right];
            leftNode.indices.reserve(static_cast<size_t>(expectedSize * 1.5));
            rightNode.indices.reserve(static_cast<size_t>(expectedSize * 1.5));
            
            // Now every entry will be on either left, right or both sides of the plane
            for (const uint32& index : lateNode.indices)
            {
                const Cullable& cull = cullData.at(index);
                float distance = lateNode.divisor.SignedDistance(cull.position);
                Insert(distance < 0 ? leftNode : rightNode, index);
            }

            // Do not keep copies
            lateNode.indices.clear();

            // Split both left and right
            Split(left, InDepth + 1);
            Split(right, InDepth + 1);
        }

        void Get(const uint32 InNodeIndex, const Vector<Vec3F>& InPoints, Set<uint32>& OutNodes) const
        {
            auto& node = nodes.at(InNodeIndex);
            if (node.left == static_cast<uint32>(-1) && node.right == static_cast<uint32>(-1))
            {
                OutNodes.insert(InNodeIndex);
                return;
            }
            
            // Get children
            CHECK_ASSERT(node.left == static_cast<uint32>(-1), "Invalid left index");
            CHECK_ASSERT(node.right == static_cast<uint32>(-1), "Invalid right index");

            switch (Query(node, InPoints))
            {
            case SpatialQueryResult::LEFT_INSIDE:
                Get(node.left, InPoints, OutNodes);
                break;
            case SpatialQueryResult::RIGHT_INSIDE:
                Get(node.right, InPoints, OutNodes);
                break;
            case SpatialQueryResult::BOTH_INSIDE:
                Get(node.left, InPoints, OutNodes);
                Get(node.right, InPoints, OutNodes);
                break;
            default:
            case SpatialQueryResult::NONE_INSIDE:
                CHECK_ASSERT(true, "At least one side has to be partially inside the shape")
                break;
            }
        }

        void GetDebug(const Node& InNode, int InDepth, const Vector<Vec3F>& InPoints, Vector<DebugNode>& OutResult) const
        {
            auto& debugNode = OutResult.emplace_back();
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
                    GetDebug(nodes.at(InNode.left), InDepth + 1, InPoints, OutResult);
                    auto& rightNode = OutResult.emplace_back();
                    rightNode.divisor = nodes.at(InNode.right).divisor;
                    rightNode.depth = InDepth + 1;
                    break;
                }
            case SpatialQueryResult::RIGHT_INSIDE:
                {
                    GetDebug(nodes.at(InNode.right), InDepth + 1, InPoints, OutResult);
                    auto& leftNode = OutResult.emplace_back();
                    leftNode.divisor = nodes.at(InNode.left).divisor;
                    leftNode.depth = InDepth + 1;
                    break;
                }
            case SpatialQueryResult::BOTH_INSIDE:
                GetDebug(nodes.at(InNode.left), InDepth + 1, InPoints, OutResult);
                GetDebug(nodes.at(InNode.right), InDepth + 1, InPoints, OutResult);
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
     };
}
