#include "Player.h"

#include "Combat/Weapon/Weapon.h"
#include "ECS/Checkpoint.h"
#include "ECS/GameEnd.h"
#include "ECS/SectionEnd.h"
#include "ECS/Systems/Attributes.h"
#include "Engine/ECS/Systems/CameraComponent.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Menu/Manager.h"
#include "../../Instances/GameInstance.h"
#include "GameState.h"
#include "ECS/Obstacles/Obstacle.h"
#include "ECS/Obstacles/Projectile.h"
#include "UI/Menus/MenuDeath.h"
#include "UI/Menus/MenuGameEnd.h"

void ECS::Player::Init()
{
    if (Engine::Instance::Get().IsEditor())
        return; 

    Input::Manager::Get().Push("Player");
    
    // Find camera and collider
    const auto& trans = Get<Transform>(GetID());
    for (const EntityID child : trans.GetChildren())
    {
        if (TryGet<Collider>(child))
            colliderID = child;
        if (TryGet<CameraComponent>(child))
            cameraID = child;
        
        auto& a = Get<Attributes>(child);
        if (a.Name.Get() == "Left")
            leftID = child;
        if (a.Name.Get() == "Right")
            rightID = child;
    }

    // Create sword
    
    const auto& state = GameState::Get();
    if (weaponID == INVALID_ID && (state.checkpoint > 0 || state.arena))
    {
        if (const auto bp = ResBlueprint("Gameplay/Player/BP_Sword.json").Get())
        {
            weaponID = bp->Instantiate();
            PickupWeapon(weaponID); 
        }
    }
}

void ECS::Player::Update()
{
    const auto& t = Get<Transform>(GetID());
    if (t.GetPosition().y < -30.0f)
        Die();
}

void ECS::Player::OnBeginContact(const Physics::Contact& InContact)
{
    const auto& t = Get<Transform>(InContact.target);
    const auto parent = t.GetParent(); 
    if (parent != INVALID_ID)
    {
        if (TryGet<Checkpoint>(parent))
            ActivateCheckpoint();
        if (TryGet<SectionEnd>(parent))
            TriggerSectionEnd();
        if (TryGet<GameEnd>(parent))
            TriggerGameEnd();
        if (TryGet<Obstacle>(parent))
            Die();
        if (TryGet<Projectile>(parent))
            Die();
        if (TryGet<Weapon>(parent))
            PickupWeapon(parent); 
    }
}

void ECS::Player::PickupWeapon(EntityID InWeaponID)
{
    const auto& swordTrans = Get<Transform>(InWeaponID);
    swordTrans.SetParent(GetID());
    weaponID = InWeaponID;
    playTimer = Utility::Timer();
    LOG("Weapon attached");
}

void ECS::Player::Die() const
{
    LOG("Player died!");
    auto& state = GameState::Get(); 
    state.deaths++;
    state.elapsedTime += playTimer.Ellapsed(); 
    Menu::Manager::Get().Push<MenuDeath>();
}

void ECS::Player::TriggerSectionEnd() const
{
    LOG("Reached section end!");
    auto& state = GameState::Get(); 
    state.elapsedTime += playTimer.Ellapsed();
    state.arena = true;

    // Go to next section
    Engine::Manager::Get().Pop();
    if (const auto newGame = Engine::Manager::Get().Push<GameInstance>())
        newGame->SetState(state); // Transfer game state
}

void ECS::Player::TriggerGameEnd() const
{
    LOG("Reached game end!");
    auto& state = GameState::Get(); 
    state.elapsedTime += playTimer.Ellapsed();
    Menu::Manager::Get().Push<MenuGameEnd>();
}

void ECS::Player::ActivateCheckpoint() const
{
    LOG("Activated checkpoint!");
    auto& state = GameState::Get(); 
    state.elapsedTime += playTimer.Ellapsed();
    state.arena = false;
    state.checkpoint++;

    // Go to next section
    Engine::Manager::Get().Pop();
    if (const auto newGame = Engine::Manager::Get().Push<GameInstance>())
        newGame->SetState(state); // Transfer game state
}
