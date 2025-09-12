#pragma once
#include "Menu/Menu.h"

class MenuRoomEditor : Menu::Instance
{
    CLASS_INFO(MenuRoomEditor, Menu::Instance);
public:
    void Init() override;
    void Update() override;
    bool IsBlocking() const override { return false; }

    void AddOption(const String &InName, const String &InText);
    String GetSelected() const { return selected; }
    void SetSelected(const String& InStr);

    UI::Container& GetUI() { return ui; }

    struct OnClickedEvent
    {
        String InOption;
    };
    
private:
    struct Option
    {
        String name;
        String text;
    };
    Vector<Option> options;
    String selected;
};
