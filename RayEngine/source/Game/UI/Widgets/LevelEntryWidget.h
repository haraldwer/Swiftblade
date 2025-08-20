#pragma once
#include "UI/Elements/Container.h"

namespace UI
{
    class LevelEntryWidget : public Container
    {
        TYPE_INFO(LevelEntryWidget, Container);
    public:
        LevelEntryWidget(const String& InLevel) : level(InLevel) {}
        
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetLevel(const String& InLevel);

        bool IsHovered() const override;
        
    private:
        String level;
    };
}
