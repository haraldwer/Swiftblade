#include "Transform.h"

#include "Engine/ECS/Manager.h"
#include "Engine/Instance/Instance.h"

using namespace ECS;

Vec3F ECS::Transform::GetPosition(Space InSpace) const
{
    switch (InSpace)
    {
    case Space::WORLD:
        return WorldMat.GetPosition(); 
    case Space::LOCAL:
        return Position;
    }
    return Vec3F::Zero(); 
}

QuatF ECS::Transform::GetRotation(Space InSpace) const
{
    switch (InSpace)
    {
    case Space::WORLD:
        return WorldMat.GetRotation(); 
    case Space::LOCAL:
        return Rotation;
    }
    return Vec3F::Zero(); 
}

Vec3F ECS::Transform::GetScale(Space InSpace) const
{
    switch (InSpace)
    {
    case Space::WORLD:
        return WorldMat.GetScale(); 
    case Space::LOCAL:
        return Scale;
    }
    return Vec3F::Zero();
}

void ECS::Transform::SetPosition(const Vec3F& InPos, Space InSpace)
{
    switch (InSpace)
    {
    case Space::WORLD:
        {
            Mat4F world = World();
            world.SetPosition(InPos); 
            SetWorld(Mat4F(world));
            break;
        }
    case Space::LOCAL:
        {
            Position = InPos;
            SetLocal(Local());
            break;
        }
    }
}

void ECS::Transform::SetRotation(const QuatF& InRot, Space InSpace)
{
    switch (InSpace)
    {
    case Space::WORLD:
        {
            Mat4F world = World();
            world.SetRotation(InRot); 
            SetWorld(Mat4F(world));
            break;
        }
    case Space::LOCAL:
        {
            Rotation = InRot;
            SetLocal(Local());
            break;
        }
    }
}

void ECS::Transform::SetScale(const Vec3F& InScale, Space InSpace)
{
    switch (InSpace)
    {
    case Space::WORLD:
        {
            Mat4F world = World();
            world.SetScale(InScale); 
            SetWorld(Mat4F(world));
            break;
        }
    case Space::LOCAL:
        {
            Scale = InScale; 
            SetLocal(Local());
            break;
        }
    }
}

void ECS::Transform::SetWorld(const Mat4F& InWorld)
{
    CHECK_ASSERT(!CanMove(), "Tried to move a static object")
    CHECK_RETURN(WorldMat == InWorld);
    WorldMat = InWorld; 
    auto& sys = Manager::Get().GetSystem<SysTransform>();
    const Mat4F local = sys.WorldToLocal(*this, InWorld);
    Position = local.GetPosition();
    Rotation = local.GetRotation();
    Scale = local.GetScale();
    sys.UpdateChildrenTransform(*this);    
}

void ECS::Transform::SetLocal(const Mat4F& InLocal)
{
    CHECK_ASSERT(!CanMove(), "Tried to move a static object")
    Position = InLocal.GetPosition();
    Rotation = InLocal.GetRotation();
    Scale = InLocal.GetScale();
    auto& sys = Manager::Get().GetSystem<SysTransform>();
    WorldMat = sys.LocalToWorld(*this, InLocal);
    sys.UpdateChildrenTransform(*this);
}

bool ECS::Transform::CanMove() const
{
    return Engine::Instance::Get().IsEditor() || !IsStatic(); 
}

void SysTransform::Init(EntityID InID, Transform& InComponent)
{
    // Update cache by setting itself
    InComponent.SetLocal(InComponent.Local());
}

void SysTransform::Deinit(EntityID InID, Transform& InComponent)
{
    // Remove parent from children
    for (const auto child : InComponent.Children)
    {
        auto& c = Get<Transform>(child);
        c.Parent = InvalidID;
        c.SetLocal(c.World());
    }

    // Remove self from parent
    if (InComponent.Parent != InvalidID)
        Get<Transform>(InComponent.Parent).Children.erase(InID);
}

bool SysTransform::Edit(EntityID InID)
{
    if (System::Edit(InID))
    {
        auto& t = Get<Transform>(InID);
        t.SetLocal(t.Local());  
        return true; 
    }
    return false; 
}

void SysTransform::SetupHierarchy(const EntityID InParent, const EntityID InChild)
{
    CHECK_RETURN_LOG(InParent == InvalidID || InChild == InvalidID, "Invalid ID");
    const auto parent = TryGet<Transform>(InParent);
    CHECK_RETURN_LOG(!parent, "Unable to find parent");
    const auto child = TryGet<Transform>(InChild);
    CHECK_RETURN_LOG(!child, "Unable to find child");
    parent->Children.insert(InChild);
    child->Parent = InParent;
    child->Static = parent->IsStatic();
    child->SetLocal(child->Local());
}

void SysTransform::UpdateChildrenTransform(const Transform& InParent)
{
    for (const EntityID child : InParent.Children)
    {
        auto& t = Get<Transform>(child);
        t.SetLocal(t.Local());
        UpdateChildrenTransform(t); 
    }
}

Mat4F SysTransform::WorldToLocal(Transform& InComp, const Mat4F& InWorld) const
{
    if (InComp.Parent != InvalidID)
    {
        const Transform& pTrans = Get<Transform>(InComp.Parent);
        return Mat4F::GetFastInverse(pTrans.World()) * InWorld;
    }
    return InWorld; 
}

Mat4F SysTransform::LocalToWorld(Transform& InComp, const Mat4F& InLocal) const
{
    if (InComp.Parent != InvalidID)
    {
        // Assume parent cache 
        const Transform& pTrans = Get<Transform>(InComp.Parent);
        return InLocal * pTrans.World();    
    }
    return InLocal; 
}
