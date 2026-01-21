#pragma once
#include <functional>

#include "Cullable.h"
#include "../../Audio/pch.h"
#include "Math/Coord.h"

namespace Utility
{
    template <class T, class KeyType, class CoordType>
    class GridVectorContainer
    {
        struct Node
        {
            Vector<T> data;
        };
        
    public:

        GridVectorContainer(int InSize) : size(InSize) {}
        
        void Insert(const T& InData, const Cullable& InCullable)
        {
            Coord c = GetCoord(InCullable);
            auto& node = nodes[c.key];
            node.data.push_back(InData);
            all.push_back(InData);
        }

        void Clear()
        {
            size_t s = nodes.size();
            nodes.clear();
            nodes.reserve(s);
        }

        Vector<const Vector<T>*> Get(const std::function<bool(const Coord<KeyType, CoordType>&, int)>& InQuery) const
        {
            Vector<const Vector<T>*> result;
            for (auto& node : nodes)
                if (InQuery(Coord<KeyType, CoordType>(node.first), size))
                    result.push_back(&node.second.data);
            return result;
        }

        const Vector<T>& GetAll() const
        {
            return all;
        }

        uint32 Count() const
        {
            return static_cast<uint32>(all.size());
        }
        
    private:
        
        Coord<KeyType, CoordType> GetCoord(const Vec3F& InPos)
        {
            return Coord(
                InPos.x / size,
                InPos.y / size,
                InPos.z / size);
        }
        
        Map<uint64, Node> nodes;
        Vector<T> all;
        int size = 1;
    };
}
        