#pragma once

class StateMachine;

class StateBase : public Utility::Typed<StateBase>
{
public:
	void SetOwner(const ObjectPtr<StateMachine>& InOwner) { Owner = InOwner; }
	
	virtual void Init() {}
	virtual Utility::TypeAny Update(double InDT) { return {}; }
	virtual Utility::TypeAny Check() { return {}; }
	
	virtual void Enter() {}
	virtual void Exit() {}
	
	virtual int32 Priority() const { return 0; }

protected:
	
	StateBase* GetCurrentState() const;
	StateBase* GetState(const Utility::TypeAny& InType) const;
	bool IsCurrentState() const;

	template <class T>
	T* GetState() const
	{
		return TypeCast<T>(GetState(T::GetType()));
	}
	
private:
	ObjectPtr<StateMachine> Owner = nullptr;
};

template <class T>
class State : public StateBase, public Utility::Typed<T>
{
}; 