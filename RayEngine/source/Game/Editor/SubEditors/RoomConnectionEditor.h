#pragma once

#include "Editor/RoomSubEditor.h"
#include "Engine/Blueprints/Blueprint.h"

struct RoomConnectionEditorConfig : BaseConfig<RoomConnectionEditorConfig>
{
    PROPERTY_C(ResBlueprint, StartBP, "Gameplay/BP_RoomConnection.json")
    PROPERTY_C(ResBlueprint, EndBP, "Gameplay/BP_RoomConnection.json");
    PROPERTY_C(Vec3F, Offset, Vec3F(-1));
    PROPERTY_C(float, LerpSpeed, 0.1f);

    PROPERTY_C(ResBlueprint, ArenaStartBP, "Gameplay/BP_RoomConnection.json");
    PROPERTY_C(ResBlueprint, CheckpointBP, "Gameplay/BP_Checkpoint.json");

    String Name() const override { return "RoomConnectionEditor"; }
};

class RoomConnectionEditor : public RoomSubEditor
{
    CLASS_INFO(RoomConnectionEditor, RoomSubEditor);
    
public:
    void Init() override;
    void Deinit() override;
    void Update() override;

    ECS::EntityID GetConnection(bool InSnap) const;

private:

    Vec3F GetOff(bool IsStart) const;

    ECS::EntityID startEntity = ECS::INVALID_ID;
    ECS::EntityID endEntity = ECS::INVALID_ID;
    RoomConnectionEditorConfig config = {};
    ECS::VolumeCoord selectCoord;
    
};
