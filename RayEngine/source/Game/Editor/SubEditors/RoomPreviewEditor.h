#pragma once

#include "../RoomSubEditor.h"

struct RoomSubmitEditorConfig : BaseConfig<RoomSubmitEditorConfig>
{
    String Name() const override { return "RoomSubmitEditor"; }
};

class RoomPreviewEditor : public RoomSubEditor
{
    // Preview decorated level
    // Maybe submit?
    // There is also some UI for this editor
    TYPE_INFO(RoomPreviewEditor, RoomSubEditor)
public:
    void Init() override;
    void Deinit() override;
    void Enter() override;
    void Exit() override;
    
};
