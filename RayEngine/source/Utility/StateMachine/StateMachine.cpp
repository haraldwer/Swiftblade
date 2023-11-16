#include "StateMachine.h"

#include <algorithm>
#include "StateBase.h"

void StateMachine::Init()
{
	Vector<StateBase*> states; // TODO: Get states!
	for (const auto& state : states)
	{
		CHECK_CONTINUE(!state);
		myStates.emplace_back(state);
		if (state->IsA(GetDefaultStateType()))
			SetStatePtr(state);
	}

	// Sort states by priority
	std::ranges::sort(myStates.begin(), myStates.end(), [](const ObjectPtr<StateBase>& InFirst, const ObjectPtr<StateBase>& InSecond)
	{
		return InFirst->Priority() < InSecond->Priority();
	});

	// Init
	for (auto& it : myStates)
	{
		if (const auto state = it.Get())
		{
			state->SetOwner(this); 
			state->Init();
		}
	}
}

void StateMachine::Update(double InDelta)
{
	// Check conditions for every state
	// Prioritize later states
	StateBase* nextState = nullptr;
	for (auto& state : myStates)
	{
		auto const ptr = state.Get();
		CHECK_CONTINUE_LOG(!ptr, "Invalid state ptr");
		if (const auto newStateType = ptr->Check())
			if (const auto newState = GetState(newStateType))
				nextState = newState;
	}

	if (nextState)
		SetStatePtr(nextState);

	// Update current state
	const auto statePtr = State.Get();
	CHECK_RETURN_LOG(!statePtr, "No current state");
	if (const auto newStateType = statePtr->Update(InDelta))
		if (const auto newState = GetState(newStateType))
			SetStatePtr(newState);
}

bool StateMachine::SetStatePtr(StateBase* InState)
{
	CHECK_RETURN(!InState, false);
	const auto currentState = State.Get();
	CHECK_RETURN(InState == currentState, false);
	if (currentState)
		currentState->Exit();
	State = InState;
	InState->Enter();
	return true;
}

bool StateMachine::TryOverrideState(StateBase* InState)
{
	CHECK_RETURN(!InState, false);
	const auto currentState = State.Get();
	CHECK_RETURN(InState == currentState, false);
	// Will still set state if equal priority!
	CHECK_RETURN(currentState && currentState->Priority() > InState->Priority(), false); 
	return SetStatePtr(InState);
}

StateBase* StateMachine::GetState(const Utility::TypeAny& InType) const
{
	CHECK_RETURN(!InType, nullptr);
	for (auto& state : myStates)
		if (const auto ptr = state.Get())
			if (ptr->IsA(InType))
				return ptr;
	return nullptr;
}
