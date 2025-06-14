#include "Transform.h"

#include "Collider.h"
#include "raylib.h"
#include "ECS/Manager.h"
#include "Editor/ImGuizmo_Wrapper.h"
#include "ImGui/Gizmo/ImGuizmo.h"
#include "Instance/Instance.h"

using namespace ECS;

Vec3F ECS::Transform::GetPosition(const Space InSpace) const
{
    switch (InSpace)
    {
    case Space::WORLD:
        return worldMat.GetPosition(); 
    case Space::LOCAL:
        return localMat.GetPosition();
    }
    return Vec3F::Zero(); 
}

QuatF ECS::Transform::GetRotation(const Space InSpace) const
{
    switch (InSpace)
    {
    case Space::WORLD:
        return worldMat.GetRotation(); 
    case Space::LOCAL:
        return localMat.GetRotation();
    }
    return QuatF::Identity();  
}

Vec3F ECS::Transform::GetScale(const Space InSpace) const
{
    switch (InSpace)
    {
    case Space::WORLD:
        return worldMat.GetScale(); 
    case Space::LOCAL:
        return localMat.GetScale();
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
            SetWorld(world, false);
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

void ECS::Transform::SetWorld(const Mat4F& InWorld, const bool InForce)
{
    CHECK_RETURN(!InForce && worldMat == InWorld);
    worldMat = InWorld; 
    auto& sys = Manager::Get().GetSystem<SysTransform>();
    localMat = sys.WorldToLocal(*this, InWorld);
    sys.UpdateChildrenTransform(*this);    
}

void ECS::Transform::SetLocal(const Mat4F& InLocal, const bool InForce)
{
    CHECK_RETURN(!InForce && localMat == InLocal);
    localMat = InLocal; 
    auto& sys = Manager::Get().GetSystem<SysTransform>();
    worldMat = sys.LocalToWorld(*this, InLocal);
    sys.UpdateChildrenTransform(*this);
}

void ECS::Transform::SetParent(const EntityID InID, const Space InSpace) const
{
    Manager::Get().GetSystem<SysTransform>().SetupHierarchy(InID, GetID(), InSpace, true); 
}

void ECS::Transform::AddChild(const EntityID InID, const Space InSpace) const
{
    Manager::Get().GetSystem<SysTransform>().SetupHierarchy(GetID(), InID, InSpace, true); 
}

void ECS::Transform::RemoveChild(const EntityID InID, const Space InSpace)
{
    Manager::Get().GetSystem<SysTransform>().SetupHierarchy(INVALID_ID, InID, InSpace, true);
}

void SysTransform::Init(EntityID InID, Transform& InComponent)
{
    // Update cache by setting itself
    InComponent.SetLocal(InComponent.Local(), true);
}

void SysTransform::Deinit(EntityID InID, Transform& InComponent)
{
    const auto copy = InComponent.children; 
    for (const auto child : copy)
        SetupHierarchy(INVALID_ID, child, Transform::Space::WORLD, false);
    SetupHierarchy(INVALID_ID, InID, Transform::Space::WORLD, false);
}

bool ECS::Transform::CustomDeserialize(const DeserializeObj& InObj)
{
    bool success = true;

    // Custom transform deserialization
    Vec3F p = localMat.GetPosition();
    QuatF r = localMat.GetRotation();
    Vec3F s = localMat.GetScale();
    if (!Utility::Deserialize(InObj, "Position", p))
        success = false;
    if (!Utility::Deserialize(InObj, "Rotation", r))
        success = false;
    if (!Utility::Deserialize(InObj, "Scale", s))
        success = false;
    SetLocal(Mat4F(p, r, s), true); 
    
    return success;
}

void ECS::Transform::CustomSerialize(SerializeObj& InOutObj) const
{
    // Custom transform serialization
    const Vec3F p = localMat.GetPosition();
    const QuatF r = localMat.GetRotation();
    const Vec3F s = localMat.GetScale();
    if (p != Vec3F::Zero())
        Utility::Serialize(InOutObj, "Position", p);
    if (r != QuatF::Identity())
        Utility::Serialize(InOutObj, "Rotation", r);
    if (s != Vec3F::One())
        Utility::Serialize(InOutObj, "Scale", s);
}

String ECS::Transform::ToString(Space InSpace) const
{
    const Mat4F mat = InSpace == Space::WORLD ? World() : Local();
    return Utility::ToStr(mat); 
}

bool SysTransform::Edit(const EntityID InID)
{
    bool edited = false;
    if (EditValues(InID))
        edited = true; 
    if (EditGizmo(InID))
        edited = true;
    return edited; 
}

bool SysTransform::EditValues(const EntityID InID)
{
    bool edited = false; 
    
    // Custom edit
    auto& t = Get<Transform>(InID);
    Vec3F p = t.localMat.GetPosition();
    QuatF r = t.localMat.GetRotation();
    Vec3F s = t.localMat.GetScale();
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

    return edited; 
}

bool SysTransform::EditGizmo(EntityID InID)
{
    // Gizmo edit
    auto& t = Get<Transform>(InID);
    static int gizmoSpace = 1;
    static int gizmoOperation = 0;
    static bool useSnap = true;

    if (!Engine::Instance::Get().GetEditorCamera().IsControlling())
    {
        if (IsKeyPressed(KEY_W))
            gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
        if (IsKeyPressed(KEY_E))
            gizmoOperation = ImGuizmo::OPERATION::ROTATE;
        if (IsKeyPressed(KEY_R))
            gizmoOperation = ImGuizmo::OPERATION::SCALE;
    }
    
    Mat4F trans = t.World();
    const bool finishEdit = ImGuizmo::Edit(trans, gizmoSpace, gizmoOperation, useSnap);
    t.SetWorld(trans); 
    return finishEdit; 
}

void SysTransform::SetupHierarchy(const EntityID InParent, const EntityID InChild, Transform::Space InSpace, bool InApplyCollider)
{
    // First get child trans
    const auto childTrans = TryGet<Transform>(InChild);
    CHECK_RETURN_LOG(!childTrans, "Unable to find child");

    // Cache mat
    Mat4F mat;
    switch (InSpace)
    {
    case Transform::Space::WORLD:
        mat = childTrans->World();
        break;
    default:
        mat = childTrans->Local();
        break;
    }

    // Remove current child parent
    if (childTrans->parent != INVALID_ID)
    {
        auto& oldParentTransform = Get<Transform>(childTrans->parent);
        oldParentTransform.children.erase(InChild);
        childTrans->parent = INVALID_ID;
    }

    // Set new parent
    if (InParent != INVALID_ID)
    {
        auto& newParentTransform = Get<Transform>(InParent);
        newParentTransform.children.insert(InChild);
        childTrans->parent = InParent;
    }
    
    // Apply cached mat
    switch (InSpace)
    {
    case Transform::Space::WORLD:
        childTrans->SetWorld(mat, true);
        break;
    default:
        childTrans->SetLocal(mat, true);
        break; 
    }

    // Apply this to physics
    if (InApplyCollider)
    {
        auto& sysCollider = GetSystem<SysCollider>();
        if (sysCollider.Contains(InChild))
            sysCollider.UpdateShape(InChild);
    }
}

void SysTransform::UpdateChildrenTransform(const Transform& InParent)
{
    for (const EntityID child : InParent.children)
    {
        auto& t = Get<Transform>(child);
        t.SetLocal(t.Local(), true);
    }
}

Mat4F SysTransform::WorldToLocal(const Transform& InComp, const Mat4F& InWorld) const
{
    if (InComp.parent != INVALID_ID)
    {
        // Assume parent cache
        const auto& pTrans = Get<Transform>(InComp.parent);
        return InWorld * Mat4F::GetFastInverse(pTrans.World());
    }
    return InWorld; 
}

Mat4F SysTransform::LocalToWorld(const Transform& InComp, const Mat4F& InLocal) const
{
    if (InComp.parent != INVALID_ID)
    {
        // Assume parent cache 
        const auto& pTrans = Get<Transform>(InComp.parent);
        return InLocal * pTrans.World();    
    }
    return InLocal; 
}
