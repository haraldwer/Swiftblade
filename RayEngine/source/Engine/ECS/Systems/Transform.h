#pragma once

#include "ECS/System.h"
#include "ECS/Component.h"

namespace Physics
{
    class Manager; 
}

namespace ECS
{
    struct Transform : Component<Transform>
    {
        friend Physics::Manager;
        
        enum class Space : uint8
        {
            LOCAL,
            WORLD
        };
        
        friend class SysTransform;
        
        Vec3F GetPosition(Space InSpace = Space::WORLD) const;
        QuatF GetRotation(Space InSpace = Space::WORLD) const;
        Vec3F GetScale(Space InSpace = Space::LOCAL) const;

        void SetPosition(const Vec3F& InPos, Space InSpace = Space::WORLD); 
        void SetRotation(const QuatF& InRot, Space InSpace = Space::WORLD); 
        void SetScale(const Vec3F& InScale, Space InSpace = Space::LOCAL); 
        
        Mat4F World() const { return worldMat; }
        Mat4F Local() const { return localMat; }

        void SetWorld(const Mat4F& InWorld, bool InForce = false);
        void SetLocal(const Mat4F& InLocal, bool InForce = false);
        
        const Set<EntityID>& GetChildren() const { return children; }
        EntityID GetParent() const { return parent; }

        void SetParent(EntityID InID, Space InSpace = Space::WORLD) const;
        void AddChild(EntityID InID, Space InSpace = Space::WORLD) const;
        static void RemoveChild(EntityID InID, Space InSpace = Space::WORLD); 
        
        bool CustomDeserialize(const DeserializeObj& InObj) override;
        void CustomSerialize(SerializeObj& InOutObj) const override;
        String ToString(Space InSpace = Space::WORLD) const;

    protected:
 
        Set<EntityID> children = {}; 
        EntityID parent = INVALID_ID;
        Mat4F worldMat = {};
        Mat4F localMat = {};
    };

    class SysTransform : public System<Transform>
    {
    public:
        void Init(EntityID InID, Transform& InComponent) override;
        void Deinit(EntityID InID, Transform& InComponent) override;
        
        bool Edit(EntityID InID) override;
        bool EditValues(EntityID InID);
        bool EditGizmo(EntityID InID); 
        
        void SetupHierarchy(EntityID InParent, EntityID InChild, Transform::Space InSpace, bool InApplyCollider);

        // Propagate changes to children  
        void UpdateChildrenTransform(const Transform& InParent);

        Mat4F WorldToLocal(const Transform& InComp, const Mat4F& InWorld) const;
        Mat4F LocalToWorld(const Transform& InComp, const Mat4F& InLocal) const;

        int GetPriority() const override { return 100; }
    };
}

