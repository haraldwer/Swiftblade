#include "RoomPreviewEditor.h"

#include "Editor/RoomEditor.h"
#include "Editor/Menus/MenuRoomPreview.h"
#include "Instance/InstanceManager.h"
#include "Menu/Manager.h"

void RoomPreviewEditor::Init()
{
    RoomSubEditor::Init();

    onClicked = { [&](auto& InE, auto InC)
    {
        if (InE.option == "Submit" && !submitting)
        {
            if (GetEditor().SubmitRoom())
            {
                previewMenu->BeginLoading();
                submitting = true;
            }
            else
            {
                previewMenu->SetSubmitState(false, "Invalid submit request");
            }
        }
        if (InE.option == "Preview")
            GetEditor().PlayRoom();
        if (InE.option == "Return")
            Engine::InstanceManager::Get().Pop();
    }};

    onSubmit = { [&](auto& InResp, auto InC)
    {
        OnSubmitResponse(InResp);
    }};
}

void RoomPreviewEditor::Deinit()
{
    RoomSubEditor::Deinit();
}

void RoomPreviewEditor::Enter()
{
    RoomSubEditor::Enter();
    previewMenu = Menu::Manager::Get().Push<MenuRoomPreview>();
}

void RoomPreviewEditor::Exit()
{
    RoomSubEditor::Exit();
    Menu::Manager::Get().Close(previewMenu);
    previewMenu = nullptr;
}

void RoomPreviewEditor::OnSubmitResponse(const DB::Response<DB::RPCSubmitRoom>& InResp)
{
    submitting = false;
    if (!InResp.success)
    {
        previewMenu->SetSubmitState(false, InResp.error);
        return;
    }
    
    if (!InResp.data.Success)
    {
        previewMenu->SetSubmitState(false, InResp.data.Error);
        return;
    }
    
    LOG("Room was submitted successfully!");
    previewMenu->SetSubmitState(true, "");
    // Remove local file!
    GetEditor().FinalizeSubmit();
}
