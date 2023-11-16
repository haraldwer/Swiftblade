#pragma once

class StateBase; 

class StateMachine
{
public:
	virtual ~StateMachine() = default;

	void Init();
	void Update(double InDelta); 
	
	// Override with default state
	virtual Utility::TypeAny GetDefaultStateType() = 0; 

	template <class T>
	bool SetState();
	bool SetStatePtr(StateBase* InState);
	bool TryOverrideState(StateBase* InState);
	
	template <class T>
	T* GetState() const;
	StateBase* GetState(const Utility::TypeAny& InType) const;
	StateBase* GetCurrentState() const { return State.Get(); };

protected:
	
	ObjectPtr<StateBase> State;
	Vector<ObjectPtr<StateBase>> myStates;
	
};

template <class T>
bool StateMachine::SetState()
{
	return SetStatePtr(GetState<T>());
}

template <class T>
T* StateMachine::GetState() const
{
	for (auto& state : myStates)
		if (auto ptr = Utility::TypeCast<T>(state.Get()))
			return ptr;
	return nullptr;
}