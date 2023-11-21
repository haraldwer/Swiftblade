#include "StateMachine.h"

#include <algorithm>
#include "StateBase.h"

void StateMachine::Init()
{
	for (StateBase* state : States)
	{
		CHECK_CONTINUE(!state);
		auto type = state->GetType(); 
		TypeMap[type] = state;
		if (type == GetDefaultStateType())
			SetState(type);
	}

	// Sort states by priority
	std::ranges::sort(States.begin(), States.end(), [](const StateBase* InFirst, const StateBase* InSecond)
	{
		if (InFirst && InSecond)
			return InFirst->Priority() < InSecond->Priority();
		return false; 
	});

	// Init
	for (StateBase* state : States)
	{
		CHECK_CONTINUE(!state)
		state->SetOwner(this); 
		state->Init();
	}
}

void StateMachine::Deinit()
{
	for (StateBase* state: States)
		if (state)
			state->Deinit();
}

void StateMachine::Update(double InDelta)
{
	// Check conditions for every state
	// Prioritize later states
	Utility::Type nextState = Utility::Type::None();
	for (StateBase* state : States)
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
	if (const Utility::Type newState = statePtr->Update(InDelta))
		SetState(newState);
}

bool StateMachine::SetState(const Utility::Type& InType)
{
	CHECK_RETURN(!InType, false);
	CHECK_RETURN(InType == CurrentState, false)
	if (const auto currentState = GetCurrentState())
		currentState->Exit();
	CurrentState = InType;
	if (const auto newState = GetCurrentState())
		newState->Enter();
	return true;
}

bool StateMachine::TryOverrideState(const Utility::Type& InType)
{
	CHECK_RETURN(!InType, false);
	CHECK_RETURN(InType == CurrentState, false);
	const auto currentState = GetCurrentState();
	const auto newState = GetState(InType);
	
	// Will still set state if equal priority!
	if (currentState && newState)
		if (currentState->Priority() > newState->Priority())
			return false; 
	
	return SetState(InType); 
}

StateBase* StateMachine::GetState(const Utility::Type& InType) const
{
	CHECK_RETURN(!InType, nullptr);
	for (StateBase* state : States)
		if (state)
			if (state->GetType() == InType)
				return state;
	return nullptr;
}
