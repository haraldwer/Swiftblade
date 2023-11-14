#include "Transform.h"

#include "Engine/ECS/Manager.h"
#include "Engine/Instance/Instance.h"
#include "ImGui/Gizmo/ImGuizmo_Wrapper.h"

using namespace ECS;

Vec3F ECS::Transform::GetPosition(const Space InSpace) const
{
    switch (InSpace)
    {
    case Space::WORLD:
        return WorldMat.GetPosition(); 
    case Space::LOCAL:
        return LocalMat.GetPosition();
    }
    return Vec3F::Zero(); 
}

QuatF ECS::Transform::GetRotation(const Space InSpace) const
{
    switch (InSpace)
    {
    case Space::WORLD:
        return WorldMat.GetRotation(); 
    case Space::LOCAL:
        return LocalMat.GetRotation();
    }
    return Vec3F::Zero(); 
}

Vec3F ECS::Transform::GetScale(const Space InSpace) const
{
    switch (InSpace)
    {
    case Space::WORLD:
        return WorldMat.GetScale(); 
    case Space::LOCAL:
        return LocalMat.GetScale();
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
            SetWorld(world);
            break;
        }
    case Space::LOCAL:
        {
            Mat4F local = Local();
            local.SetPosition(InPos);
            SetLocal(local);
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
            SetWorld(world);
            break;
        }
    case Space::LOCAL:
        {
            Mat4F local = Local();
            local.SetRotation(InRot);
            SetLocal(local);
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
            SetWorld(world);
            break;
        }
    case Space::LOCAL:
        {
            Mat4F local = Local();
            local.SetScale(InScale); 
            SetLocal(local);
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
    LocalMat = sys.WorldToLocal(*this, InWorld);
    sys.UpdateChildrenTransform(*this);    
}

void ECS::Transform::SetLocal(const Mat4F& InLocal, bool bInForce)
{
    CHECK_ASSERT(!bInForce && !CanMove(), "Tried to move a static object")
    LocalMat = InLocal; 
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
    InComponent.SetLocal(InComponent.Local(), true);
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

bool ECS::Transform::Deserialize(const DeserializeObj& InObj)
{
    bool success = true;
    if (!Component::Deserialize(InObj))
        success = false; 

    // Custom transform deserialization
    Vec3F p = LocalMat.GetPosition();
    QuatF r = LocalMat.GetRotation();
    Vec3F s = LocalMat.GetScale();
    if (!Utility::Deserialize(InObj, "Position", p))
        success = false;
    if (!Utility::Deserialize(InObj, "Rotation", r))
        success = false;
    if (!Utility::Deserialize(InObj, "Scale", s))
        success = false;
    SetLocal(Mat4F(p, r, s), true); 
    
    return success;
}

void ECS::Transform::Serialize(SerializeObj& InOutObj) const
{
    Component::Serialize(InOutObj);

    // Custom transform serialization
    const Vec3F p = LocalMat.GetPosition();
    const QuatF r = LocalMat.GetRotation();
    const Vec3F s = LocalMat.GetScale();
    if (p != Vec3F::Zero())
        Utility::Serialize(InOutObj, "Position", p);
    if (r != QuatF::Identity())
        Utility::Serialize(InOutObj, "Rotation", r);
    if (s != Vec3F::One())
        Utility::Serialize(InOutObj, "Scale", s);
}

bool SysTransform::Edit(const EntityID InID)
{
    bool edited = false; 
    
    // Custom edit
    auto& t = Get<Transform>(InID);
    Vec3F p = t.LocalMat.GetPosition();
    QuatF r = t.LocalMat.GetRotation();
    Vec3F s = t.LocalMat.GetScale();
    if (Utility::Edit("P", p) ||
        Utility::Edit("R", r) ||
        Utility::Edit("S", s))
    {
        t.SetLocal(Mat4F(p, r, s));
        edited = true; 
    }

    // Default edit
    if (System::Edit(InID))
        edited = true;

    // Gizmo
    static int gizmoSpace = 1;
    static int gizmoOperation = 0;
    static bool useSnap = true; 
    Mat4F trans = t.World(); 
    if (ImGuizmo::Edit(trans, gizmoSpace, gizmoOperation, useSnap))
    {
        t.SetWorld(trans); 
        edited = true;
    }
    
    return edited; 
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
    child->SetLocal(child->Local(), true);
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

Mat4F SysTransform::WorldToLocal(const Transform& InComp, const Mat4F& InWorld) const
{
    if (InComp.Parent != InvalidID)
    {
        const auto& pTrans = Get<Transform>(InComp.Parent);
        return Mat4F::GetFastInverse(pTrans.World()) * InWorld;
    }
    return InWorld; 
}

Mat4F SysTransform::LocalToWorld(const Transform& InComp, const Mat4F& InLocal) const
{
    if (InComp.Parent != InvalidID)
    {
        // Assume parent cache 
        const auto& pTrans = Get<Transform>(InComp.Parent);
        return InLocal * pTrans.World();    
    }
    return InLocal; 
}
