#pragma once

#include "../RoomSubEditor.h"
#include "Resources/Material.h"
#include "Resources/Model.h"

struct RoomPathEditorConfig : BaseConfig<RoomPathEditorConfig>
{
    PROPERTY_C(ResModel, PathPoint, "Defaults/M_Sphere.obj");
    PROPERTY_C(ResModel, PathLink, "Defaults/M_Cylinder.obj");
    PROPERTY_C(ResRM, PathMaterial, "Editor/RM_EditorPathLink.json");
    PROPERTY_C(float, Scale, 0.5f);
    
    String Name() const override { return "RoomPathEditor"; }
};

class RoomPathEditor : public RoomSubEditor
{
    TYPE_INFO(RoomPathEditor, RoomSubEditor);
    
public:
    void Init() override;
    void Deinit() override;
    void Update() override;
    void Frame() override;

private:
    void VerifyPath();
    
    RoomPathEditorConfig config;
    uint32 pointID = 0;
    uint32 linkID = 0;
    uint32 pointHash = 0;
    uint32 linkHash = 0;
    bool renderCacheChanged = false;
};
