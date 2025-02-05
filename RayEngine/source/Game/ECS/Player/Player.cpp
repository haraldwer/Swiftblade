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
#include "GameInstance.h"
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
            ColliderID = child;
        if (TryGet<CameraComponent>(child))
            CameraID = child;
        
        auto& a = Get<Attributes>(child);
        if (a.Name.Get() == "Left")
            LeftID = child;
        if (a.Name.Get() == "Right")
            RightID = child;
    }

    // Create sword
    
    const auto& state = GameState::Get();
    if (WeaponID == InvalidID && (state.Checkpoint > 0 || state.InArena))
    {
        if (auto bp = ResBlueprint("Gameplay/Player/BP_Sword.json").Get())
        {
            WeaponID = bp->Instantiate();
            PickupWeapon(WeaponID); 
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
    const auto& t = Get<Transform>(InContact.Target);
    const auto parent = t.GetParent(); 
    if (parent != InvalidID)
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
    auto& swordTrans = Get<Transform>(InWeaponID);
    swordTrans.SetParent(GetID());
    WeaponID = InWeaponID;
    PlayTimer = Utility::Timer();
    LOG("Weapon attached");
}

void ECS::Player::Die()
{
    LOG("Player died!");
    auto& state = GameState::Get(); 
    state.Deaths++;
    state.ElapsedTime += PlayTimer.Ellapsed(); 
    Menu::Manager::Get().Push<MenuDeath>();
}

void ECS::Player::TriggerSectionEnd()
{
    LOG("Reached section end!");
    auto& state = GameState::Get(); 
    state.ElapsedTime += PlayTimer.Ellapsed();
    state.InArena = true;

    // Go to next section
    Engine::Manager::Get().Pop();
    if (const auto newGame = Engine::Manager::Get().Push<GameInstance>())
        newGame->SetState(state); // Transfer game state
}

void ECS::Player::TriggerGameEnd()
{
    LOG("Reached game end!");
    auto& state = GameState::Get(); 
    state.ElapsedTime += PlayTimer.Ellapsed();
    Menu::Manager::Get().Push<MenuGameEnd>();
}

void ECS::Player::ActivateCheckpoint()
{
    LOG("Activated checkpoint!");
    auto& state = GameState::Get(); 
    state.ElapsedTime += PlayTimer.Ellapsed();
    state.InArena = false;
    state.Checkpoint++;

    // Go to next section
    Engine::Manager::Get().Pop();
    if (const auto newGame = Engine::Manager::Get().Push<GameInstance>())
        newGame->SetState(state); // Transfer game state
}
