#include "ButtonTab.h"

#include "LabelHeader.h"
#include "UI/Builder.h"
#include "UI/Elements/Image.h"

void UI::ButtonTab::Init(Container &InOwner)
{
    text = Add(LabelHeader({}, option));
    underline = Add(Element({
            .size = { 10, 2 } ,
            .anchor = { 0, 1 }
        }, {
            .color = Vec4F::One(),
            .cornerRadius = 2.0f,
        }));
    
    Container::Init(InOwner);
}

void UI::ButtonTab::Update(Container &InOwner)
{
    Container::Update(InOwner);

    auto& u = Get<Element>(underline);
    auto t = u.GetTransform();
    auto b = u.GetBackground();
    t.size.x = Get<Label>(text).GetDesiredSize().x;
    Vec2F desiredPos = transform.position;
    float desiredAlpha = 0.0f;
    
    if (IsHovered())
    {
        desiredPos = Vec2F(0, 3);
        desiredAlpha = 0.7f;
    }
    
    if (IsPressed())
    {
        desiredPos = Vec2F(0, 5);
        desiredAlpha = 1.0f;
    }
    
    if (selected)
    {
        desiredPos = Vec2F(0, 0);
        desiredAlpha = 1.0f;
    }
    
    t.position = Utility::Math::Lerp(t.position, desiredPos, 0.1f);
    b.color.a = Utility::Math::Lerp(b.color.a, desiredAlpha, 0.1f);
    u.SetTransform(t);
    u.SetBackground(b);
}

void UI::ButtonTab::SetSelected(bool InSelected)
{
    selected = InSelected;
}
