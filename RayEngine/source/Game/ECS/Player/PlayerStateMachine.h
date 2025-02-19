#pragma once
#include "BaseConfig.h"
#include "Utility/StateMachine/StateMachine.h"

class PlayerStateMachine : public StateMachine, public BaseConfig<PlayerStateMachine>
{
public:
	~PlayerStateMachine() override = default;
	void Init() override;
    String Name() const override { return "StateMachine"; }
    bool Edit(const String& InName = "", uint32 InOffset = 0) override;
    void CustomSerialize(SerializeObj& InOutObj) const override;
    bool CustomDeserialize(const DeserializeObj& InObj) override;
	
};
