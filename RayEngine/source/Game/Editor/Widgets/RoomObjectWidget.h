#pragma once
#include "UI/Elements/Container.h"
#include "UI/Elements/Label.h"

namespace UI
{
    class RoomObjectWidget : public Label
    {
        CLASS_INFO(RoomObjectWidget, Label);
    public:
        RoomObjectWidget(const String& InEntry) : Label({}, { .text = InEntry }) {}
        // TODO: Also show some kind of icon?
    };
    
}
