#pragma once

class StateBase; 

class StateMachine
{
public:
	virtual ~StateMachine() = default;

	virtual void Init();
	virtual void Deinit();
	virtual void Update(); 
	
	// Override with default state
	virtual Utility::Type GetDefaultStateType() = 0;

	bool SetState(const Utility::Type& InType);
	bool TryOverrideState(const Utility::Type& InType);
	
	template <class T>
	T* GetState() const;
	StateBase* GetState(const Utility::Type& InType) const;
	Utility::Type GetCurrentType() const { return CurrentState; };
	StateBase* GetCurrentState() const { return GetState(GetCurrentType()); };

protected:
	
	Utility::Type CurrentState = Utility::Type::None();
	Vector<StateBase*> States;
	Map<Utility::TypeHash, StateBase*> TypeMap;
};

template <class T>
T* StateMachine::GetState() const
{
	auto find = TypeMap.find(Utility::GetType<T>());
	if (find != TypeMap.end())
		return reinterpret_cast<T*>(find->second); 
	return nullptr;
}