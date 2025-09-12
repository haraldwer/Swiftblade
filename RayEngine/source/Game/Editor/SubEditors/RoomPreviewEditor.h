#pragma once

#include "../RoomSubEditor.h"
#include "Database/Events.h"
#include "Database/Data/RPCSubmit.h"
#include "Editor/Menus/MenuRoomPreview.h"

struct RoomSubmitEditorConfig : BaseConfig<RoomSubmitEditorConfig>
{
    String Name() const override { return "RoomSubmitEditor"; }
};

class RoomPreviewEditor : public RoomSubEditor
{
    CLASS_INFO(RoomPreviewEditor, RoomSubEditor)
public:
    void Init() override;
    void Deinit() override;
    void Enter() override;
    void Exit() override;

private:
    
    void OnSubmitResponse(const DB::Response<DB::RPCSubmitRoom>& InResp);
    
    InstanceEvent<MenuRoomPreview::OnClickedEvent>::Callback onClicked;
    MenuRoomPreview* previewMenu = nullptr;
    
    DB::Event<DB::RPCSubmitRoom>::Callback onSubmit;
    bool submitting = false; 
};
