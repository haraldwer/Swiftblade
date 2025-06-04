#include "StateMachine.h"

#include "StateBase.h"
#include <algorithm>

#include "ImGui/imgui.h"

void StateMachine::Init()
{
	for (StateBase* state : states)
	{
		CHECK_CONTINUE(!state);
		Utility::Type type = state->GetType(); 
		typeMap[type.GetHash()] = state;
		if (type == GetDefaultStateType())
			SetState(type);
	}

	// Sort states by priority
	std::ranges::sort(states.begin(), states.end(), [](const StateBase* InFirst, const StateBase* InSecond)
	{
		if (InFirst && InSecond)
			return InFirst->Priority() < InSecond->Priority();
		return false; 
	});

	// Init
	for (StateBase* state : states)
	{
		CHECK_CONTINUE(!state)
		state->SetOwner(this); 
		state->Init();
	}
}

void StateMachine::Deinit()
{
	for (StateBase* state: states)
		if (state)
			state->Deinit();
}

void StateMachine::Update()
{
	PROFILE();
	
	// Check conditions for every state
	// Prioritize later states
	Utility::Type nextState = Utility::Type::None();
	for (StateBase* state : states)
	{
		CHECK_CONTINUE_LOG(!state, "Invalid state ptr");
		if (const Utility::Type newStateType = state->Check())
			nextState = newStateType; 
	}

	if (nextState)
		SetState(nextState);

	// Update current state
	StateBase* statePtr = GetCurrentState();
	CHECK_RETURN_LOG(!statePtr, "No current state");
	if (const Utility::Type newState = statePtr->Update())
		SetState(newState);
}

bool StateMachine::SetState(const Utility::Type& InType)
{
	CHECK_RETURN(!InType, false);
	CHECK_RETURN(InType == currentState, false)

	const Type prevState = currentState;

	// When exiting previous state, current state should be new state
	currentState = InType; 
	if (const auto current = GetState(prevState))
		current->Exit();

	// When entering new state, current state should be previous state
	currentState = prevState;
	if (const auto newState = GetState(InType))
		newState->Enter();

	// Finally set current state
	currentState = InType; 
	return true;
}

bool StateMachine::TryOverrideState(const Utility::Type& InType)
{
	CHECK_RETURN(!InType, false);
	CHECK_RETURN(InType == currentState, false);
	const auto current = GetCurrentState();
	const auto newState = GetState(InType);
	
	// Will still set state if equal priority!
	if (current && newState)
		if (current->Priority() > newState->Priority())
			return false; 
	
	return SetState(InType); 
}

StateBase* StateMachine::GetState(const Utility::Type& InType) const
{
	CHECK_RETURN(!InType, nullptr);
	for (StateBase* state : states)
		if (state)
			if (state->GetType() == InType)
				return state;
	return nullptr;
}
