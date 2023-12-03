#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Transform : Component<Transform>
    {
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
        
        Mat4F World() const { return WorldMat; }
        Mat4F Local() const { return LocalMat; }

        void SetWorld(const Mat4F& InWorld, bool InForce = false);
        void SetLocal(const Mat4F& InLocal, bool InForce = false);
        
        const Set<EntityID>& GetChildren() const { return Children; }
        EntityID GetParent() const { return Parent; }

        void SetParent(EntityID InID, Space InSpace = Space::WORLD) const;
        void AddChild(EntityID InID, Space InSpace = Space::WORLD) const;
        void RemoveChild(EntityID InID, Space InSpace = Space::WORLD) const; 
        
        bool Deserialize(const DeserializeObj& InObj) override;
        void Serialize(SerializeObj& InOutObj) const override;

    protected:
 
        Set<EntityID> Children; 
        EntityID Parent = InvalidID;
        Mat4F WorldMat;
        Mat4F LocalMat;
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

