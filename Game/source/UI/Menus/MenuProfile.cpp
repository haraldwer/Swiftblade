#include "MenuProfile.h"

#include "Database/Manager.h"
#include "Database/Components/Authentication.h"
#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"
#include "UI/Widgets/Common/LabelText.h"
#include "UI/Widgets/Common/LabelTitle.h"

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
            .Push(UI::List(UI::Transform::Fill(20)), "Page")
                .Add(UI::LabelTitle({}, "username"), "Username")
                .Add(UI::LabelText(), "Plays")
                .Add(UI::LabelText(), "Kills")
                .Add(UI::LabelText(), "FirstLogin");
    
    ui = builder.Build();
    ui["Page"].SetVisible(false);
    
    RequestUserInfo();
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
    onRecieveInfo.Bind([&](auto InResp, auto InC)
    {
        CHECK_RETURN(!InResp.success)
        LOG("Received userinfo")

        // Show page
        ui["Page"].SetVisible(true);
        ui["Loading"].SetVisible(false);
        
        // Update info
        ui.Get<UI::Label>("Username").SetText(InResp.data.Username);
        ui.Get<UI::Label>("Plays").SetText(Utility::ToStr(InResp.data.Plays));
        ui.Get<UI::Label>("Kills").SetText(Utility::ToStr(InResp.data.Kills));
        ui.Get<UI::Label>("FirstLogin").SetText(InResp.data.FirstLogin);

        DB::RPCUserLB::Request lbReq;
        lbReq.User = user;
        lbReq.LB = 1;
        DB::Manager::Get().rpc.Request<DB::RPCUserLB>(lbReq);
        
        DB::RPCUserFavs::Request favsReq;
        DB::Manager::Get().rpc.Request<DB::RPCUserFavs>(favsReq);
        DB::RPCUserSubmission::Request subReq;
        DB::Manager::Get().rpc.Request<DB::RPCUserSubmission>(subReq);
    });

    onRecieveLB.Bind([&](auto InResp, auto InC)
    {
        CHECK_RETURN(!InResp.success)
        LOG("Received lb")
    });
    
    onRecieveFavs.Bind([&](auto InResp, auto InC)
    {
        CHECK_RETURN(!InResp.success)
        LOG("Received favs")
    });
    
    onRecieveSubmissions.Bind([&](auto InResp, auto InC)
    {
        CHECK_RETURN(!InResp.success)
        LOG("Received submissions")
    });
    
    DB::RPCUserInfo::Request req;
    req.User = user;
    DB::Manager::Get().rpc.Request<DB::RPCUserInfo>(req);
    
}
