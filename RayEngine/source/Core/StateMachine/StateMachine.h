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
	virtual Utility::Type GetDefaultStateType() { return {}; };

	bool SetState(const Utility::Type& InType);
	bool TryOverrideState(const Utility::Type& InType);
	
	template <class T>
	T* GetState() const;
	StateBase* GetState(const Utility::Type& InType) const;
	Utility::Type GetCurrentType() const { return currentState; };
	StateBase* GetCurrentState() const { return GetState(GetCurrentType()); };

protected:
	
	Utility::Type currentState = Utility::Type::None();
	Vector<StateBase*> states;
	Map<Utility::TypeHash, StateBase*> typeMap;
};

template <class T>
T* StateMachine::GetState() const
{
	auto find = typeMap.find(Type::Get<T>().GetHash());
	if (find != typeMap.end())
		return reinterpret_cast<T*>(find->second); 
	return nullptr;
}