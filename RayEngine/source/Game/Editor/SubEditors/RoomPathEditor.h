#pragma once

#include "../RoomSubEditor.h"
#include "Engine/Blueprints/Blueprint.h"

struct RoomPathEditorConfig : BaseConfig<RoomPathEditorConfig>
{
    PROPERTY_C(ResBlueprint, StartBP, "Gameplay/BP_RoomConnection.json");
    PROPERTY_C(ResBlueprint, EndBP, "Gameplay/BP_RoomConnection.json");

    PROPERTY_C(ResBlueprint, ArenaStartBP, "Gameplay/BP_RoomConnection.json");
    PROPERTY_C(ResBlueprint, CheckpointBP, "Gameplay/BP_Checkpoint.json");

    String Name() const override { return "RoomConnectionEditor"; }
};

class RoomPathEditor : public RoomSubEditor
{
public:
    void Init() override;
    void Deinit() override;
    void Update() override;
    void Frame() override;
    void DebugDraw() override;
    
    Mat4F GetStartOffset() const;

private:
    
    RoomPathEditorConfig config = {};
    ECS::EntityID endEntity = ECS::INVALID_ID;
    ECS::EntityID startEntity = ECS::INVALID_ID;
    Vec3F targetPos = {};
    Vec3F orgPos = {};

    ECS::EntityID GetCurrent() const;
    static Mat4F GetTrans(ECS::EntityID InID);
};
