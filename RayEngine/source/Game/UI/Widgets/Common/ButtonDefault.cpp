#include "ButtonDefault.h"

#include "LabelHeader.h"
#include "LabelText.h"
#include "UI/Builder.h"

void UI::ButtonDefault::Init(Container &InOwner)
{
    Container::Init(InOwner);

    Builder b = Builder(Container({ .margins = { 10, 5 }}))
        .Add(LabelHeader({}, Utility::ToUpper(text)));
    root = Add(b.Build());

    Get<Element>(root).SetBackground({ Vec4F::One() });
}

void UI::ButtonDefault::Update(Container &InOwner)
{
    Container::Update(InOwner);

    auto& r = Get<Element>(root);
    auto t = r.GetTransform();
    auto b = r.GetBackground();
    
    Vec2F desiredPos = transform.position;
    float desiredAlpha = 0.5f;
    
    if (IsHovered())
    {
        desiredPos = Vec2F(0, -1);
        desiredAlpha = 0.7f;
    }
    
    if (IsPressed())
    {
        desiredPos = Vec2F(0, 1);
        desiredAlpha = 0.9f;
    }
    
    t.position = Utility::Math::Lerp(t.position, desiredPos, 0.1f);
    b.color.a = Utility::Math::Lerp(b.color.a, desiredAlpha, 0.1f);
    
    r.SetTransform(t);
    r.SetBackground(b);
}
