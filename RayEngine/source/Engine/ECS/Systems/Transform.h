#pragma once

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Transform : Component<Transform>
    {
        friend class SysTransform;
        
    public:
        Vec3F GetPosition() const { return Position; }
        QuatF GetRotation() const { return Rotation; }
        Vec3F GetScale() const { return Scale; }

        void SetPosition(const Vec3F& InPos); 
        void SetRotation(const QuatF& InRot); 
        void SetScale(const Vec3F& InScale); 
        
        bool IsStatic() const { return Static; }
        Mat4F Matrix() const
        {
            return Mat4F(
                Position,
                Rotation,
                Scale);
        }
        
    protected:
        PROPERTY(Vec3F, Position);
        PROPERTY(QuatF, Rotation);
        PROPERTY_P(Vec3F, Scale, Vec3F::One());
        PROPERTY_P(bool, Static, false); 

        // Load these? 
        Set<EntityID> Children; 
        EntityID Parent = InvalidID; 
    };

    class SysTransform : public System<Transform>
    {
    public:
        // TODO: Set up hierarchy!
        void SetupHierarchy(EntityID InParent, EntityID InChild);
    };
}

