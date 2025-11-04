#include "ButtonDefault.h"

#include "LabelHeader.h"
#include "LabelText.h"
#include "UI/Builder.h"

void UI::ButtonDefault::Init(Container &InOwner)
{
    Container::Init(InOwner);
    
    String text = properties.text.empty() ? properties.option : properties.text;
    Builder b = Builder(Container({ .margins = { 10, 5 }}))
        .Add(LabelHeader({}, Utility::ToUpper(text)));
    root = Add(b.Build());

    Get<Element>(root).SetBackground({ Vec4F::One() });
}

void UI::ButtonDefault::Update(Container &InOwner)
{
    Container::Update(InOwner);
    
    if (IsPressed())
        PressAnim();
    else if (IsHovered())
        HoverAnim();
    else DefaultAnim();
}

void UI::ButtonDefault::HoverAnim()
{
    LerpTowards({0, -1}, properties.hoverColor, properties.hoverSpeed);
}

void UI::ButtonDefault::PressAnim()
{
    LerpTowards({0, 1}, properties.pressColor, properties.pressSpeed);
}

void UI::ButtonDefault::DefaultAnim()
{
    LerpTowards({0, 0}, properties.defaultColor, properties.defaultSpeed);
}

void UI::ButtonDefault::LerpTowards(Vec2F InPos, Vec4F InCol, float InSpeed)
{
    auto& r = Get<Element>(root);
    auto t = r.GetTransform();
    auto b = r.GetBackground();
    t.position = Utility::Math::Lerp(t.position, InPos, InSpeed);
    b.color = Utility::Math::Lerp(b.color, InCol, InSpeed);
    r.SetTransform(t);
    r.SetBackground(b);
}
