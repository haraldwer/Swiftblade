#include "InfoPanelLevel.h"

#include "Database/Data/RPCLevelInfo.h"
#include "Instance/Manager.h"
#include "Instances/GameInstance.h"
#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/List.h"
#include "UI/Elements/TabContainer.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"
#include "UI/Widgets/Common/TextboxDefault.h"
#include "UI/Widgets/LevelList/LevelEntryWidget.h"

void UI::InfoPanelLevel::Init(Container &InOwner)
{
    Container::Init(InOwner);

    Builder b = Builder(Container({
                .alignment = 1,
                .margins = 10
            }))
        .Push(List())
            .Push(TabContainer(), "NameTab")
                .Push(List({}, { 10, ListDirection::HORIZONTAL}), "NameShow")
                    .Add(ButtonDefault({ .anchor = 0.5, .pivot = 0.5 }, " "), "EditName")
                    .Push(List({ .anchor = { 0, 0.5 }, .pivot = { 0, 0.5 }}))
                        .Add(LabelHeader(), "Name")
                        .Add(LabelText({.padding = {0, {-5, 0}}}), "Creator")
                    .Pop()
                .Pop()
                .Add(TextboxDefault({
                        .alignment = { 1, 1 }
                    }, {
                        .maxChars = 25,
                        .centering = { 0, 0.5 },
                    }), "NameEdit")
            .Pop()
            .Add(LabelHeader(), "Rooms")
            .Push(List(Transform::Fill(10), {}, {{ 0, 0, 0, 0.5 }}), "RoomList")
                // Add rooms here    
            .Pop()
            .Add(LabelHeader(), "Arenas")
            .Push(List(Transform::Fill(10), {}, {{ 0, 0, 0, 0.5 }}), "ArenaList")
                // Add arenas here
            .Pop()
        .Pop()
        .Push(List({
                .position = {},
                .anchor = {1, 1},
                .pivot = {1, 1}
            },  {
                .direction = ListDirection::HORIZONTAL
            }))
            .Add(ButtonDefault({}, "Play"), "Play");
    Add(b.Build());

    Get<TabContainer>("NameTab").Set("NameShow");

    onInfo.Bind([](const auto& InData, auto InC)
    {
        InC->RecieveInfo(InData);
    });
}

void UI::InfoPanelLevel::Update(Container &InOwner)
{
    Container::Update(InOwner);

    if (Get<ButtonDefault>("EditName").IsClicked())
        Get<TabContainer>("NameTab").Set("NameEdit");

    auto& tbx = Get<Textbox>("NameEdit");
    if (tbx.IsCommitted())
    {
        String text = tbx.GetText();
        if (text != data.entry.Name.Get())
        {
            if (text.empty())
                text = "Untitled";

            if (auto res = data.resource.Get())
            {
                res->data.Name = text;
                res->Save();
            }

            data.entry.Name = text;
            InstanceEvent<LevelEntryData>::Invoke(data);
            
            // Apply new name!
            Get<Label>("Name").SetText(text);
        }
        
        Get<TabContainer>("NameTab").Set("NameShow");
    }
    
    if (Get<ButtonDefault>("Play").IsClicked())
    {
        if (auto game = Engine::Manager::Get().Push<GameInstance>())
        {
            // Set level!
        }
    }
}

void UI::InfoPanelLevel::SetLevel(const LevelEntryData &InData)
{
    if (!data.entry.ID.Get().empty() && data.entry.ID == InData.entry.ID)
        return;
    if (data.resource.Identifier().IsValid() && data.resource == InData.resource)
        return;
    
    data = InData;

    String name = data.entry.Name;
    String creator = data.entry.Creator;

    // Not a local file, check cache
    if (!data.resource.Identifier().IsValid() && !data.entry.ID.Get().empty())
        data.resource = Utility::GetCachePath(data.entry.ID, ".json");
    
    if (auto res = data.resource.Get())
    {
        name = res->data.Name;
        creator = res->data.Creator;

        // Show all the resource info!
    }
    else
    {
        // Request from server
        DB::RPCLevelInfo::Request request;
        request.ID = data.entry.ID;
        DB::Manager::Get().rpc.Request<DB::RPCLevelInfo>(request);

        // Show loading
    }

    if (name.empty())
        name = "Untitled";
    if (creator.empty())
        creator = "you";
    Get<Label>("Name").SetText(name);
    Get<Textbox>("NameEdit").SetText(name);
    Get<Label>("Creator").SetText("by " + creator);
}

void UI::InfoPanelLevel::RecieveInfo(const DB::Response<DB::RPCLevelInfo> &InResponse)
{
    if (!InResponse.success)
    {
        LOG("RPC failed")
        return;
    }

    if (InResponse.data.ID != data.entry.ID)
    {
        LOG("ID mismatch");
        return;
    }
    
    LOG("Received info!")
    
    // Create cache level
    String path = Utility::GetCachePath(InResponse.data.ID, ".json");
    if (!InResponse.data.Level.Get().Save(path))
    {
        LOG("Failed to save: " + path)
        return;
    }

    // Set the cache path
    data.resource = path;
}
