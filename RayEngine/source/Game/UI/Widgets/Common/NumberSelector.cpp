#include "NumberSelector.h"

#include "ButtonDefault.h"
#include "LabelText.h"
#include "TextboxDefault.h"
#include "UI/Builder.h"
#include "UI/Elements/List.h"
#include "UI/Elements/Textbox.h"

void UI::NumberSelector::Init(Container &InOwner)
{
    Container::Init(InOwner);

    value = properties.defaultValue;
    
    Builder b = Builder()
        .Add(LabelText({
                .anchor = { 0, 0.5 },
                .pivot = { 0, 0.5 }
            }, properties.text))
        .Push(List({
                .alignment = 1,
                .anchor = 1,
                .pivot = 1 },
            {
                .spacing = 5,
                .direction = ListDirection::HORIZONTAL,
                .reversed = true
            }))
            .Add(ButtonDefault(Transform::SimpleAnchor(0.5, 0.5), "+"), "Plus")
            .Add(TextboxDefault(Transform::SimpleAnchor(0.5, 0.5), { .defaultText = Utility::ToStr(value)}), "Value")
            .Add(ButtonDefault(Transform::SimpleAnchor(0.5, 0.5), "-"), "Minus");
    Add(b.Build());
}

void UI::NumberSelector::Update(Container &InOwner)
{
    Container::Update(InOwner);
    changed = false;

    if (Get<Element>("Minus").IsClicked())
        ChangeValue(-properties.stepSize);
    if (Get<Element>("Plus").IsClicked())
        ChangeValue(properties.stepSize);

    auto& tbx = Get<Textbox>("Value");
    if (tbx.IsCommitted())
        SetValue(std::stof(tbx.GetText()));
}

void UI::NumberSelector::ChangeValue(const float InDelta)
{
    SetValue(value + InDelta);
}

void UI::NumberSelector::SetValue(const float InValue)
{
    float prevValue = value;
    value = InValue;
    if (properties.limits != Vec2F::Zero())
        value = Utility::Math::Clamp(value, properties.limits.x, properties.limits.y);
    if (properties.roundPrecision > 0.000001f)
        value = roundf(value * properties.roundPrecision) / properties.roundPrecision;
    if (value != prevValue)
    {
        Get<Textbox>("Value").SetText(Utility::ToStr(value));
        changed = true;
    }
}

float UI::NumberSelector::GetValue() const
{
    return value;
}
