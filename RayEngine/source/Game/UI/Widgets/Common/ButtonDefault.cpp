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
    float lerpSpeed = 0.1f;
    
    if (IsPressed())
    {
        desiredPos = Vec2F(0, 1);
        desiredAlpha = 0.9f;
    }
    else if (IsHovered())
    {
        desiredPos = Vec2F(0, -1);
        desiredAlpha = 0.7f;
        lerpSpeed = 0.9f;
    }
    
    t.position = Utility::Math::Lerp(t.position, desiredPos, lerpSpeed);
    b.color.a = Utility::Math::Lerp(b.color.a, desiredAlpha, lerpSpeed);
    
    r.SetTransform(t);
    r.SetBackground(b);
}
