#pragma once
#include "Resources/Texture.h"
#include "UI/Elements/Container.h"

namespace UI
{
    class RoomObjectWidget : public Container
    {
        CLASS_INFO(RoomObjectWidget, Container);
    public:
        RoomObjectWidget(const String& InEntry, const ResTexture& InIcon) : Container({}, {}), Entry(InEntry), Icon(InIcon) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;

    private:
        String Entry;
        ResTexture Icon;
    };
    
}
