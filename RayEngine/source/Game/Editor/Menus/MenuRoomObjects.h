#pragma once
#include "Editor/SubEditors/RoomObjectEditor.h"
#include "Instance/Instance.h"
#include "UI/Elements/Element.h"

class MenuRoomObjects : public Menu::Instance
{
    CLASS_INFO(MenuRoomObjects, Menu::Instance);
public:
    void Init() override;
    void Deinit() override;
    void Update() override;

    bool IsBlocking() const override { return false; }
    bool CanBePopped() const override { return false; }
    
    void SetConfig(const RoomObjectEditorConfig& InConfig);
    String TryPick();

private:
    void Focus(const String& InName);
    
    struct Entry
    {
        String name;
        UI::ElementID id;
    };
    Vector<Entry> objects;
    
    float targetOffset = 0.0f;
    float scrollOffset = 0.0f;
    const float scrollSensitivity = 1.0f;
    const float scrollSpeed = 0.1f;
    const float totalSpacing = 1000.0f;
    
    const float minVerticalOffset = -80.0f;
    const float maxVerticalOffset = 80.0f;
    const float offsetPow = 3.0f;
};
