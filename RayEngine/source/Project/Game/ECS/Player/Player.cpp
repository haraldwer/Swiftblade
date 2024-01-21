#include "Player.h"

#include "PlayerInput.h"
#include "Engine/ECS/Systems/Camera.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Menu/Manager.h"
#include "Game/GameInstance.h"
#include "Game/GameState.h"
#include "Game/ECS/Checkpoint.h"
#include "Game/ECS/GameEnd.h"
#include "Game/ECS/SectionEnd.h"
#include "Game/UI/Menus/MenuGameEnd.h"
#include "Game/UI/Menus/MenuDeath.h"
#include "Weapon/Weapon.h"

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
        if (TryGet<Camera>(child))
            CameraID = child; 
    }

    // Create sword
    
    const auto& state = GameState::Get();
    if (SwordID == InvalidID && (state.Checkpoint > 0 || state.InArena))
    {
        if (auto bp = ResBlueprint("Gameplay/Player/BP_Sword.json").Get())
        {
            SwordID = bp->Instantiate();
            PickupSword(SwordID); 
        }
    }
}

void ECS::Player::Update(double InDelta)
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
        if (GetSystem<SysCheckpoint>().Contains(parent))
            ActivateCheckpoint();
        if (GetSystem<SysSectionEnd>().Contains(parent))
            TriggerSectionEnd();
        if (GetSystem<SysGameEnd>().Contains(parent))
            TriggerGameEnd();
        if (TryGet<Weapon>(parent))
            PickupSword(parent); 
    }
}

void ECS::Player::PickupSword(EntityID InSwordID)
{
    // Attach sword to player with an offset
    auto& swordTrans = Get<Transform>(InSwordID);
    swordTrans.SetParent(GetCameraID());
    swordTrans.SetPosition(Vec3F(-1.5f, 0.0f, 1.5f), Transform::Space::LOCAL);
    swordTrans.SetRotation(QuatF::FromEuler(Vec3F(0.0, 0.0, 0.2f)), Transform::Space::LOCAL);
    SwordID = InSwordID;
    PlayTimer = Utility::Timer();
    LOG("Sword attached");
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
