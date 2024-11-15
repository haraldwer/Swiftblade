#pragma once

#include "../RoomSubEditor.h"
#include "Engine/BaseConfig.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Property/Property.h"

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
    void Frame(bool InIsCameraControlling) override;
    void DebugDraw(bool InIsCameraControlling) override;
    
    Mat4F GetStartOffset() const;

private:
    
    RoomPathEditorConfig Config;
    ECS::EntityID EndEntity = ECS::InvalidID;
    ECS::EntityID StartEntity = ECS::InvalidID;
    Vec3F TargetPos;
    Vec3F OrgPos;

    ECS::EntityID GetCurrent() const;
    static Mat4F GetTrans(ECS::EntityID InID);
};
