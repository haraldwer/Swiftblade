#include "ButtonEditorTab.h"

#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/Label.h"

UI::ButtonEditorTab::ButtonEditorTab(const String &InOption) : Container()
{
    option = InOption;
    text = Add(Label(option));
    const Transform t = {
        .size = { 10, 5 } ,
        .anchor = { 0, 1 }
    };
    underline = Add(Image(ResTexture("Defaults/T_Missing.png"), t));
}

void UI::ButtonEditorTab::Update(Container &InOwner)
{
    Container::Update(InOwner);

    auto& u = Get<Image>(underline);
    auto t = u.GetTransform();
    t.size.x = Get<Label>(text).GetDesiredSize().x;
    Vec2F desiredPos = transform.position; 
    if (IsHovered())
        desiredPos = Vec2F(0, 5);
    if (IsPressed())
        desiredPos = Vec2F(0, 7);
    if (selected)
        desiredPos = Vec2F(0, 9);
    t.position = Utility::Math::Lerp(t.position, desiredPos, 0.1f);
    u.SetTransform(t);
}

void UI::ButtonEditorTab::SetSelected(bool InSelected)
{
    selected = InSelected;
}
