#include "MenuRoomObjects.h"

#include "raylib.h"
#include "Editor/Widgets/RoomObjectWidget.h"

void MenuRoomObjects::Init()
{
}

void MenuRoomObjects::Deinit()
{
    Instance::Deinit();
}

void MenuRoomObjects::Update()
{
    Instance::Update();

    float delta = GetMouseWheelMove();
    targetOffset += delta * scrollSensitivity;
    scrollOffset = Utility::Math::Lerp(scrollOffset, roundf(targetOffset), scrollSpeed);


    // Targetoffset grows
    // We need to mod

    

    float spacing = totalSpacing / static_cast<float>(objects.size());
    for (int i = 0; i < static_cast<int>(objects.size()); i++)
    {
        auto& object = objects[i];
        float offset = (scrollOffset + i) * spacing;

        // TODO: Make index wrap around?
        // TODO: Rotational offset

        float offsetPart = offset * 2 / totalSpacing;
        float heightLerp = powf(abs(offsetPart), offsetPow);
        float height = Utility::Math::Lerp(minVerticalOffset, maxVerticalOffset, heightLerp);
        
        UI::Transform t {
            .position = { offset, height },
            .anchor = {0.5, 1 },
            .pivot = {0.5, 0.5},
        };
        auto& w = ui.Get<UI::RoomObjectWidget>(object.id);
        w.SetTransform(t);
    }
}

void MenuRoomObjects::SetConfig(const RoomObjectEditorConfig& InConfig)
{
    ui.ClearChildren();
    objects.clear();
    for (auto& e : InConfig.ObjectTypes.Get())
    {
        auto& entry = objects.emplace_back();
        entry.name = e.first;
        auto w = UI::RoomObjectWidget(e.first);
        w.Init(ui);
        entry.id = ui.Add(w);
    }
}

String MenuRoomObjects::TryPick()
{
    for (auto& o : objects)
        if (ui.Get<UI::RoomObjectWidget>(o.id).IsHovered())
            return o.name;
    return {};
}

void MenuRoomObjects::Focus(const String& InName)
{
    for (int i = 0; i < static_cast<int>(objects.size()); i++)
        if (objects[i].name == InName)
            targetOffset = i;
}
