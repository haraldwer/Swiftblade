#include "Projectile.h"

#include "ECS/Manager.h"
#include "ECS/Systems/Attributes.h"
#include "ECS/Systems/Transform.h"
#include "Physics/Contact.h"

void SysProjectile::Init(ECS::EntityID InID, Projectile& InComponent)
{
    InComponent.timer = InComponent.Lifetime;
}

void SysProjectile::Update(ECS::EntityID InID, Projectile& InComponent)
{
    auto& t = Get<ECS::Transform>(InID);
    const Vec3F f = t.World().Forward();
    Vec3F p = t.GetPosition();
    p += f * InComponent.Speed.Get() * static_cast<float>(Utility::Time::Get().Delta());
    t.SetPosition(p);
    
    InComponent.timer -= Utility::Time::Get().Delta();
    if (InComponent.timer < 0.0f)
        ECS::Manager::Get().DestroyEntity(InID);
}

void SysProjectile::OnBeginContact(const Physics::Contact& InContact)
{
    LOG("Projectile collision: " + Get<ECS::Attributes>(InContact.target).Name.Get());
    ECS::Manager::Get().DestroyEntity(InContact.self);
}
