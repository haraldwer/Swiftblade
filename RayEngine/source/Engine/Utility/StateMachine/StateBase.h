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
	
	virtual bool EditState() { return false; }
	virtual void SerializeState(SerializeObj& InOutObj) const {}
	virtual bool DeserializeState(const DeserializeObj& InObj) { return false; }
	
	virtual void Enter() {}
	virtual void Exit() {}
	
	virtual int32 Priority() const { return 0; }
	virtual Utility::Type GetType() const { return {}; }
	virtual String GetName() const { return ""; }

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
class State : public StateBase, public PropertyOwner<T>
{
public:
	~State() override = default;
	
	bool EditState() override
	{
		return this->Edit("##" + std::to_string(typeid(T).hash_code()));
	}

	void SerializeState(SerializeObj& InOutObj) const override
	{
		return this->Serialize(InOutObj);
	}
	
	bool DeserializeState(const DeserializeObj& InObj) override
	{
		return this->Deserialize(InObj);
	}
	
	Utility::Type GetType() const override
	{
		return Utility::GetType<T>();
	}
	
	String GetName() const override
	{
		return String(typeid(T).name()).substr(String("class ").length());
	}
};