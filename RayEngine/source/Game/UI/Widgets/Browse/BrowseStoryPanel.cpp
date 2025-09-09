#include "BrowseStoryPanel.h"

#include <ranges>

#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"

void UI::BrowseStoryPanel::Init(Container &InOwner)
{
    BrowsePanel::Init(InOwner);
    
    conf.LoadConfig();
    
    auto b = Builder()
        .Push(List());
    
    for (auto& e : std::ranges::reverse_view(conf.Entries.Get()))
    {
        b.Add(Element({
                .size = { 5, 50},
                .anchor = 0.5,
                .pivot = 0.5,
            }, {
                .color = { 1 },
                .cornerRadius = 5
            }));
        b.Add(Label({
                .position = { 0, 0 },
                .size = { 100, 80},
                .anchor = 0.5,
                .pivot = 0.5,
            }, {
                e.Name,
                80,
                0.5,
                ResFont("UI/F_LinLibertine_aSZI.ttf")
            }), e.Name);
    }
    
    root = Add(b.Build());
}
