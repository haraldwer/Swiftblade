#include "PlayerSpawner.h"

#include "GameState.h"
#include "Player.h"
#include "ECS/Systems/Transform.h"
#include "Instance/Instance.h"

void ECS::PlayerSpawner::Init()
{
    UniqueComponent::Init();

    if (Engine::Instance::Get().IsEditor())
        return;
    
    GameState& state = GameState::Get();
    CHECK_RETURN_LOG(state.playerID != INVALID_ID && TryGet<Player>(state.playerID), "Player already exists");
    auto bp = BP.Get().Get();
    CHECK_RETURN_LOG(!bp, "Failed to get blueprint resource")
    state.playerID = bp->Instantiate(Get<Transform>().World());
    CHECK_ASSERT(state.playerID == INVALID_ID, "Failed to create player!");
}
