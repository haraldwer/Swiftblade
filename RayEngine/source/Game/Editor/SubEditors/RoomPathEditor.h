#pragma once

#include "../RoomSubEditor.h"
#include "Resources/Material.h"
#include "Resources/Model.h"

struct RoomPathEditorConfig : BaseConfig<RoomPathEditorConfig>
{
    PROPERTY_C(ResModel, PathPoint, "Defaults/M_Sphere.obj");
    PROPERTY_C(ResModel, PathLink, "Defaults/M_Cylinder.obj");
    PROPERTY_C(ResRM, PathMaterial, "Editor/RM_EditorPathLink.json");
    PROPERTY_C(float, Scale, 0.3f);
    PROPERTY_C(float, SelectedScale, 0.6f);
    PROPERTY_C(float, DotThreshold, 0.8f);
    PROPERTY_C(float, LerpSpeed, 0.1f);
    PROPERTY_C(float, MinLerpDist, 0.05f);
    
    String Name() const override { return "RoomPathEditor"; }
};

class RoomPathEditor : public RoomSubEditor
{
    TYPE_INFO(RoomPathEditor, RoomSubEditor);
    
public:
    void Init() override;
    void Deinit() override;


    void Update() override;
    void SmoothPath();
    
    void Enter() override;
    void Exit() override;
    void Frame() override;

private:
    void VerifyPath() const;
    
    RoomPathEditorConfig config;
    Vector<Vec3F> smoothPath;
    bool renderCacheChanged = false;
    uint32 pointID = 0;
    uint32 linkID = 0;
    uint32 pointHash = 0;
    uint32 linkHash = 0;

    void SelectClosest();
    void MoveSelected();
    void RemoveHovered();
    int selectedIndex = -1;
    Vec3F selectPos;
    Vector<ECS::VolumeCoordKey> selectPath;
};
