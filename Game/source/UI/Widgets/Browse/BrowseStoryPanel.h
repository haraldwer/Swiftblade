#pragma once
#include "BrowsePanel.h"
#include "Level/Level.h"

struct StoryEntry : PropertyOwner<StoryEntry>
{
    PROPERTY(String, Name);
    PROPERTY(LevelConfig, Level); 
};

struct StoryConfig : BaseConfig<StoryConfig>
{
    PROPERTY_C(Vector<StoryEntry>, Entries, {});
    String Name() const override { return "Story"; }
};

namespace UI
{
    class BrowseStoryPanel : public BrowsePanel
    {
        CLASS_INFO(BrowseStoryPanel, BrowsePanel)
    public:
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        bool DebugDraw(Container &InOwner, const String &InIdentifier, int &InC) override;

    private:
        StoryConfig config;
    };
}
