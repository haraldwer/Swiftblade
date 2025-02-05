#include "Projectile.h"

#include "ECS/Manager.h"
#include "ECS/Systems/Attributes.h"
#include "ECS/Systems/Transform.h"
#include "Physics/Contact.h"

void SysProjectile::Init(ECS::EntityID InID, Projectile& InComponent)
{
    InComponent.Timer = InComponent.Lifetime;
}

void SysProjectile::Update(ECS::EntityID InID, Projectile& InComponent)
{
    ECS::Transform& t = Get<ECS::Transform>(InID);
    Vec3F f = t.World().Forward();
    Vec3F p = t.GetPosition();
    p += f * InComponent.Speed.Get() * static_cast<float>(Utility::Time::Get().Delta());
    t.SetPosition(p);
    
    InComponent.Timer -= Utility::Time::Get().Delta();
    if (InComponent.Timer < 0.0f)
        ECS::Manager::Get().DestroyEntity(InID);
}

void SysProjectile::OnBeginContact(const Physics::Contact& InContact)
{
    LOG("Projectile collision: " + Get<ECS::Attributes>(InContact.Target).Name.Get());
    ECS::Manager::Get().DestroyEntity(InContact.Self);
}
