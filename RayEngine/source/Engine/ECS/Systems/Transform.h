#pragma once

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Transform : Component<Transform>
    {
        enum class Space : uint8
        {
            WORLD,
            LOCAL
        };
        
        friend class SysTransform;
        
    public:
        
        Vec3F GetPosition(Space InSpace = Space::WORLD) const;
        QuatF GetRotation(Space InSpace = Space::WORLD) const;
        Vec3F GetScale(Space InSpace = Space::LOCAL) const;

        void SetPosition(const Vec3F& InPos, Space InSpace = Space::WORLD); 
        void SetRotation(const QuatF& InRot, Space InSpace = Space::WORLD); 
        void SetScale(const Vec3F& InScale, Space InSpace = Space::LOCAL); 
        
        Mat4F World() const { return WorldMat; }
        Mat4F Local() const
        {
            return Mat4F(
                Position,
                Rotation,
                Scale);
        }

        void SetWorld(const Mat4F& InWorld);
        void SetLocal(const Mat4F& InLocal);
        
        const Set<EntityID>& GetChildren() const { return Children; }
        EntityID GetParent() const { return Parent; }

        bool IsStatic() const { return Static.Get(); }

    protected:

        bool CanMove() const; 
        
        PROPERTY(Vec3F, Position);
        PROPERTY(QuatF, Rotation);
        PROPERTY_P(Vec3F, Scale, Vec3F::One()); 
        PROPERTY_P(bool, Static, false); 
 
        Set<EntityID> Children; 
        EntityID Parent = InvalidID;
        Mat4F WorldMat;
    };

    class SysTransform : public System<Transform>
    {
    public:
        void Init(EntityID InID, Transform& InComponent) override;
        void Deinit(EntityID InID, Transform& InComponent) override;
        bool Edit(EntityID InID) override;
        void SetupHierarchy(EntityID InParent, EntityID InChild);

        // Propagate changes to children  
        void UpdateChildrenTransform(const Transform& InParent);

        Mat4F WorldToLocal(Transform& InComp, const Mat4F& InWorld) const;
        Mat4F LocalToWorld(Transform& InComp, const Mat4F& InLocal) const;
    };
}

