#pragma once
#include <functional>

#include "Cullable.h"

namespace Utility
{
    template <class T>
    class GridContainer
    {
        struct Coord
        {
            Coord()
            {
            }
    
            Coord(const int16 InX, const int16 InY, const int16 InZ)
            {
                pos.x = InX;
                pos.y = InY;
                pos.z = InZ;
                pos.padding = 0; 
            }

            Coord(const uint64 InKey) : key(InKey) {}

            struct Vec
            {
                int16 x;
                int16 y;
                int16 z;
                int16 padding; 
            };
        
            union
            {
                Vec pos;
                uint64 key = 0; 
            };
        };
        
        struct Node
        {
            Vector<T> data;
        };
        
    public:

        GridContainer(int InSize) : size(InSize) {}
        
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

        Vector<const Vector<T>*> Get(const std::function<bool(const Coord&, int)>& InQuery) const
        {
            Vector<const Vector<T>*> result;
            for (auto& node : nodes)
                if (InQuery(Coord(node.first), size))
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
        
        Coord GetCoord(const Vec3F& InPos)
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
        