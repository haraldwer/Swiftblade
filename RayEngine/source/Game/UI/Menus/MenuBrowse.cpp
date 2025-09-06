#include "MenuBrowse.h"

#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/SplitContainer.h"
#include "UI/Widgets/BrowseCommunityPanel.h"
#include "UI/Widgets/BrowseStoryPanel.h"
#include "UI/Widgets/BrowseCreatePanel.h"
#include "UI/Widgets/InfoPanel.h"

void MenuBrowse::Init()
{
    auto b = UI::Builder()
        .Push(UI::SplitContainer({
                .size = { 1200, 0 },
                .alignment = { 0, 1 },
                .anchor = { 0.5, 0 },
                .pivot = { 0.5, 0 }
            }, UI::SplitDirection::HORIZONTAL, 10, { 3, 2 }));
    
#define BROWSE_ADD_TYPE(x) \
    if (Utility::Type::Get<UI::x>() == panelType) \
        b.Add(UI::x(), "Content");

    BROWSE_ADD_TYPE(BrowseStoryPanel)
    BROWSE_ADD_TYPE(BrowseCommunityPanel)
    BROWSE_ADD_TYPE(BrowseCreatePanel)

#undef BROWSE_ADD_TYPE
    
    b.Add(UI::InfoPanel(), "Info");
    ui = b.Build();
}

void MenuBrowse::Update()
{
    Instance::Update();
}
