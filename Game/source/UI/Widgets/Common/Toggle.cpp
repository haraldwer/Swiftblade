#include "Toggle.h"

void UI::Toggle::Update(Container &InOwner)
{
    if (IsClicked())
        toggled = !toggled;
    
    ButtonDefault::Update(InOwner);
}

void UI::Toggle::DefaultAnim()
{
    if (toggled)
        LerpTowards({0, 0}, toggleProperties.toggleColor, properties.defaultSpeed);
    else
        LerpTowards({0, 0}, properties.defaultColor, properties.defaultSpeed);
}

void UI::Toggle::HoverAnim()
{
    if (toggled)
        LerpTowards({0, 0}, toggleProperties.toggleHoverColor, properties.hoverSpeed);
    else
        LerpTowards({0, 0}, properties.hoverColor, properties.hoverSpeed);
}
