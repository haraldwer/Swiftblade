#pragma once
#include "BrowsePanel.h"
#include "Levels/LevelConfig.h"

struct StoryEntry : PropertyOwner<StoryEntry>
{
    PROPERTY_C(String, Name, "");
    PROPERTY_C(ResScene, Scene, ""); // TODO: Replace with level config 
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
        TYPE_INFO(BrowseStoryPanel, BrowsePanel)
    public:
        void Init(Container &InOwner) override;

    private:
        StoryConfig conf;
    };
}
