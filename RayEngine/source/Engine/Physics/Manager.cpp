#include "Manager.h"

#include "ECS/Manager.h"

#include "ECS/Systems/Collider.h"
#include "ECS/Systems/Rigidbody.h"
#include "ECS/Systems/Transform.h"

#include "CollisionShape.h"

void Physics::Manager::Init() {}
void Physics::Manager::Deinit() {}
void Physics::Manager::Update() const {}
void Physics::Manager::Add(ECS::EntityID InID) {}
void Physics::Manager::Remove(ECS::EntityID InID) {}
void Physics::Manager::AddCubes(ECS::EntityID InID, const Vector<Mat4F> &InTransforms, float InScale) {}
void Physics::Manager::ClearCubes(ECS::EntityID InID) {}
void * Physics::Manager::CreateMaterial(float InStaticFric, float InDynamicFric, float InRestitution) {}
void Physics::Manager::SetTransforms() const {}
void Physics::Manager::Simulate() const {}
void Physics::Manager::GetTransforms() const {}
ECS::Rigidbody * Physics::Manager::FindRigidbody(ECS::EntityID InID) {}
void * Physics::Manager::CreateDynamic(ECS::Rigidbody &InRigidbody) {}
void * Physics::Manager::CreateStatic(const ECS::Collider &InCollider) {}
void Physics::Manager::CreateShape(const ECS::Collider &InCollider, ECS::EntityID InActorID, int &InActor) {}
void * Physics::Manager::GetGeometry(const Shape &InShape, const Vec4F &InShapeData, const Vec3F &InScale) {}
void Physics::Manager::TryReleaseShape(ECS::EntityID InID) {}
void Physics::Manager::TryReleaseDynamic(ECS::EntityID InID) {}
void Physics::Manager::TryReleaseStatic(ECS::EntityID InID) {}
