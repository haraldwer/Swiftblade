#include "MenuProfile.h"

#include "Database/Manager.h"
#include "Database/Components/Authentication.h"
#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/Label.h"

void MenuProfile::Init()
{
    UI::Builder builder = UI::Builder()
        .Push(UI::Container(
            {
                .position = { 0, -400 },
                .size = { 800,  800 },
                .anchor = { 0.5, 1 },
                .pivot = 0.5
            }))
            .Add(UI::Image({
                    .size = 50,
                    .anchor = 0.5,
                    .pivot = 0.5,
                }), "Loading")
            .Push(UI::Container(UI::Transform::Fill(20)), "Page")
                .Add(UI::Label({}, "username", 50, ResFont("UI/F_LinLibertine_aSZI.ttf")), "Username");
    
    ui = builder.Build();
    ui["Page"].SetVisible(false);

    RequestUserInfo();
    ApplyUserInfo();
}

void MenuProfile::Update()
{
    Instance::Update();

    auto& l = ui["Loading"];
    auto t = l.GetTransform();
    t.rotation = Utility::Time::Get().Total() * 100;
    l.SetTransform(t);
}

void MenuProfile::RequestUserInfo()
{
    // Assume self if nothing else specified
}

void MenuProfile::ApplyUserInfo()
{
    auto& auth = DB::Manager::Get().auth; 
    if (auth.IsAuthenticated())
    {
        LOG("Received user info, applying!")
        ui["Page"].SetVisible(true);
        ui["Loading"].SetVisible(false);
        ui.Get<UI::Label>("Username").SetText(auth.GetUsername());
    }
}
