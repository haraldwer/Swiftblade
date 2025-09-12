#include "MenuLogin.h"

#include "MenuMain.h"
#include "Database/Manager.h"
#include "Database/Components/Authentication.h"
#include "Instance/Manager.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"

void MenuLogin::Init()
{
    UI::Builder b = UI::Builder()
        .Push(UI::List(UI::Transform::Centered()), "Loading")
            .Add(UI::LabelHeader({}, "Connecting..."), "Status")
        .Pop()
        .Push(UI::List(UI::Transform::Centered()), "Fail")
            .Add(UI::LabelHeader({}, "Login failed"))
            .Add(UI::LabelText({}, "Error: Unknown"), "Error")
            .Add(UI::ButtonDefault({}, "Retry"), "Retry")
        .Pop()
        .Push(UI::List(UI::Transform::Centered()), "NameSelect")
            .Add(UI::LabelHeader({}, "Select username"))
            .Add(UI::LabelText({}, "TestUser"), "TbxName")
            .Add(UI::ButtonDefault({}, "Select"), "Select");

    ui = b.Build();

    onAuth.Bind([&](auto InResp, auto InC)
    {
        loading = false;
        if (InResp.success)
            LoginStep();
        else
            SetLoginFailed(InResp.error);
    });
    
    onLogin.Bind([&](auto InResp, auto InC)
    {
        loading = false;
        if (InResp.success)
        {
            if (InResp.data.LoggedIn)
            {
                Menu::Manager::Get().Pop();
                Menu::Manager::Get().Push<MenuMain>();
            }
            else
            {
                if (InResp.data.Name.Get().empty())
                    BeginSelectUsername();
                else
                    SetLoginFailed("User: " + InResp.data.Name.Get());
            }
        }
        else
        {
            SetLoginFailed(InResp.error);
        }
    });

    loading = false;
    LoginStep();
}

void MenuLogin::Update()
{
    Instance::Update();

    if (ui["Retry"].IsClicked())
        LoginStep();
    if (ui["Select"].IsClicked())
        LoginStep(); 
}

void MenuLogin::LoginStep()
{
    CHECK_RETURN_LOG(loading, "Login already in progress...");

    String name;
    if (ui["NameSelect"].GetVisible())
        name = ui.Get<UI::Label>("TbxName").GetText();
    auto& status = ui.Get<UI::Label>("Status");
    
    ui["Loading"].SetVisible(true);
    ui["Fail"].SetVisible(false);
    ui["NameSelect"].SetVisible(false);
    
    auto& db = DB::Manager::Get();
    if (!db.auth.IsAuthenticated())
    {
        loading = true;
        status.SetText("Authenticating...");
        db.auth.AuthenticateSteam();
        return;
    }

    if (!db.user.IsLoggedIn())
    {
        loading = true;
        status.SetText("Logging in...");
        db.user.TryLogin(name);
        return;
    }

    CHECK_ASSERT(true, "Should never reach this point");
}

void MenuLogin::BeginSelectUsername()
{
    ui["NameSelect"].SetVisible(true);
    ui["Loading"].SetVisible(false);
    ui["Fail"].SetVisible(false);
}

void MenuLogin::SetLoginFailed(const String &InError)
{
    LOG("Login failed: " + InError)

    ui["Fail"].SetVisible(true);
    ui["NameSelect"].SetVisible(false);
    ui["Loading"].SetVisible(false);
    
    ui.Get<UI::Label>("Status").SetText("Login failed");
    ui.Get<UI::Label>("Error").SetText(InError);
}
