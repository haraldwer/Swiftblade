#pragma once
#include "UI/Elements/Container.h"

namespace UI
{
    struct NumberSelectorProperties
    {
        String text = "Number";
        float defaultValue = 0.0f;
        float stepSize = 1.0f;
        float roundPrecision = 1.0f;
        Vec2F limits = Vec2F::Zero();
    };
    
    class NumberSelector : public Container
    {
        CLASS_INFO(NumberSelector, Container);

    public:
        NumberSelector() = default;
        NumberSelector(const Transform& InTrans, const NumberSelectorProperties& InProperties) : Container(InTrans), properties(InProperties) {}
        void Init(Container &InOwner) override;
        
        void Update(Container &InOwner) override;
        void ChangeValue(float InDelta);
        void SetValue(float InValue);
        float GetValue() const;
        int GetValueInt() const { return static_cast<int>(roundf(value)); }
        bool IsChanged() const { return changed; }

    private:
        NumberSelectorProperties properties;
        float value = 0.0f;
        bool changed = false;
    };
}
