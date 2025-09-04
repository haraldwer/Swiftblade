#pragma once
#include "UI/Elements/Container.h"

namespace UI
{
    // Fill this with whatever should be displayed
    struct LevelEntry
    {
        String id = "";
        String name = "unknown";
        String creator = "unknown";
        bool fav = false;
        int plays;
    };
    
    class LevelEntryWidget : public Container
    {
        TYPE_INFO(LevelEntryWidget, Container);
    public:
        LevelEntryWidget(const LevelEntry& InLevel) : level(InLevel) {}
        
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetLevel(const LevelEntry& InLevel);

        bool IsHovered() const override;
        
    private:
        LevelEntry level;
    };
}
