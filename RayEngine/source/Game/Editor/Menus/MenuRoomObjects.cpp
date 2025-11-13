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
    float size = static_cast<float>(objects.size());

    

    float spacing = totalSpacing / static_cast<float>(objects.size());
    for (int i = 0; i < static_cast<int>(objects.size()); i++)
    {
        auto& object = objects[i];
        float halfsize = size / 2;
        float index = scrollOffset + i - size * 100.0f;
        float modi = fmodf(index - halfsize, size) + halfsize;
        float offset = modi * spacing;

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

void MenuRoomObjects::SetConfig(const RoomObjectEditorConfig &InConfig, const RoomInfo &InInfo)
{
    ui.ClearChildren();
    objects.clear();

    const String& type = InInfo.Type.Get();
    auto& data = InConfig.Types.Get();
    if (!data.contains(type))
        return;
    
    auto& roomTypeData = data.at(type);
    for (auto& e : roomTypeData.Objects.Get())
    {
        auto& entry = objects.emplace_back();
        entry.name = e.first;
        auto w = UI::RoomObjectWidget(e.first, e.second.Icon);
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
