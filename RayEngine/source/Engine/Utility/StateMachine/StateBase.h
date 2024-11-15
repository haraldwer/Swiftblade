#pragma once

class StateMachine;

class StateBase
{
public:
	virtual ~StateBase() = default;
	void SetOwner(const ObjectPtr<StateMachine>& InOwner) { Owner = InOwner; }
	
	virtual void Init() {}
	virtual void Deinit() {}
	virtual Utility::Type Update() { return {}; }
	virtual Utility::Type Check() { return {}; }
	
	virtual void Enter() {}
	virtual void Exit() {}
	
	virtual int32 Priority() const { return 0; }
	virtual Utility::Type GetType() const { return {}; }

protected:
	
	Utility::Type GetCurrentState() const;
	StateBase* GetState(const Utility::Type& InType) const;
	bool IsCurrentState() const;

	template <class T>
	T* GetState() const
	{
		if (auto state = GetState(Utility::GetType<T>()))
			return reinterpret_cast<T*>(state);
		return nullptr; 
	}

private:

	ObjectPtr<StateMachine> Owner; 
};

template <class T>
class State : public StateBase
{
public:
	Utility::Type GetType() const override
	{
		return Utility::GetType<T>();
	}
};