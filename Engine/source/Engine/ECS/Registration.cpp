#include "Registration.h"

#include "Engine/ECS/Systems/Attributes.h"
#include "Engine/ECS/Systems/Camera.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Mesh.h"
#include "Engine/ECS/Systems/Environment.h"
#include "Engine/ECS/Systems/Light.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/ECS/Systems/Animator.h"
#include "Engine/ECS/Systems/Particle.h"

void ECS::RegisterEngineSystems()
{
    ECS_REG_ENTITY_SYSTEM(Attributes);
    ECS_REG_ENTITY_SYSTEM(Transform);
    ECS_REG_ENTITY_SYSTEM(Mesh);
    ECS_REG_ENTITY_SYSTEM(Environment);
    ECS_REG_ENTITY_SYSTEM(Light);
    ECS_REG_ENTITY_SYSTEM(Particle);
    ECS_REG_ENTITY_SYSTEM(Camera);
    ECS_REG_ENTITY_SYSTEM(Collider);
    ECS_REG_ENTITY_SYSTEM(Rigidbody);
    ECS_REG_ENTITY_SYSTEM(Animator);
}
