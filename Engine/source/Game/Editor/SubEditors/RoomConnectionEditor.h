#pragma once

#include "Editor/RoomSubEditor.h"
#include "Engine/Blueprints/Blueprint.h"

typedef Map<String, ResBlueprint> RoomConnectionMap;

struct RoomConnectionEditorConfig : BaseConfig<RoomConnectionEditorConfig>
{
    PROPERTY(RoomConnectionMap, StartBP)
    PROPERTY(RoomConnectionMap, EndBP);
    
    PROPERTY_C(Vec3F, Offset, Vec3F(-1));
    PROPERTY_C(float, LerpSpeed, 0.1f);

    // Different connection type based on room type
    // Same as for object editor
    

    String Name() const override { return "RoomConnectionEditor"; }
};

class RoomConnectionEditor : public RoomSubEditor
{
    CLASS_INFO(RoomConnectionEditor, RoomSubEditor);
    
public:
    void Init() override;
    void Deinit() override;
    void Update() override;
    void DebugDraw() override;

    ECS::EntityID GetEnd(bool InSnap) const;
    ECS::EntityID GetStart() const;
    Mat4F GetWorldOffset() const;

private:

    Vec3F GetOff(bool IsStart) const;

    ECS::EntityID startEntity = ECS::INVALID_ID;
    ECS::EntityID endEntity = ECS::INVALID_ID;
    RoomConnectionEditorConfig config = {};
    ECS::VolumeCoord selectCoord;
    
};
