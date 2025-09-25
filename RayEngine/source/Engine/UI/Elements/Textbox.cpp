#include "Textbox.h"

#include "raylib.h"

UI::Textbox::Textbox(const Transform &InTransform, const TextboxProperties &InProperties, const Background &InBg) : Container(InTransform, InBg, true), properties(InProperties)
{
    text = InProperties.defaultText; 
    label = Add<Label>(Label({
        .alignment = 0,
        .anchor = InProperties.centering,
        .pivot = InProperties.centering,
    }, InProperties.label));
    transform.margins = 5;
}

void UI::Textbox::Update(Container &InOwner)
{
    Container::Update(InOwner);

    CHECK_RETURN(!cacheVisible);
    
    committed = false;

    if (IsClicked())
    {
        focused = true;
        // Send UI focus event?
        // Capture all input
    }

    auto& l = Get<Label>(label);
    
    // Capture input
    while (true)
    {
        unsigned int pressed = GetCharPressed();
        if (pressed == 0)
            break;

        // Add char
        String newText = text;
        newText += pressed;
        
        if (static_cast<int>(newText.length()) > properties.maxChars && properties.maxChars > 0)
            continue;
        
        Vec2F size = l.MeasureText(newText + "|");
        float width = transform.size.x - transform.margins.horizontal.x - transform.margins.horizontal.y;
        if (properties.wrap && properties.multiline)
        {
            while (size.x > width)
            {
                // Line break?
                // Find last space
                auto find = newText.find_last_of(" ");
                if (find == std::string::npos)
                {
                    newText = text; // Found nowhere to break line, ignore char
                    break; 
                }
                
                newText =
                    newText.substr(0, find) + "\n" +
                    newText.substr(find);
                LOG("Split: " + newText);
                size = l.MeasureText(newText + "|");
            }
            text = newText;
        }
        else if (size.x < width || width <= 0)
            text = newText;
    }

    while (true)
    {
        unsigned int key = GetKeyPressed();
        if (key == 0)
            break;

        if (key == KEY_ESCAPE)
            focused = false;
        if (key == KEY_BACKSPACE)
            text = text.substr(0, text.size() - 1);
        if (key == KEY_ENTER)
        {
            if (IsKeyDown(KEY_LEFT_SHIFT) && properties.multiline)
            {
                text += "\n";
            }
            else
            {
                committed = true;
            }
        }
    }
    
    String showText = text;
    if (focused)
    {
        cursorTime -= static_cast<float>(Utility::Time::Get().Unscaled());
        if (cursorTime < 0)
        {
            cursorTime = 0.5;
            cursorShown = !cursorShown && focused;
        }
        
        if (cursorShown)
            showText += "|";
        else
            showText += " ";
    }
    else
    {
        if (showText.empty())
            showText = properties.emptyText;
    }
    
    
    l.SetText(showText);
}

bool UI::Textbox::IsHovered() const
{
    return Container::IsHovered() || Element::IsHovered();
}
