#pragma once
#include "RoomSubEditor.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"

struct RoomConnectionEditorConfig : PropertyOwner<RoomConnectionEditorConfig>
{
    PROPERTY_C(ResBlueprint, EndBP, "Blueprints/BP_RoomEnd.json");
    
    inline static const String Path = "Configs/C_RoomConnectionEditor.json";
    void LoadConfig() { Load(Path); }
    void SaveConfig() const { Save(Path); }
};

class RoomConnectionEditor : RoomSubEditor
{
public:
    void Init();
    void Deinit();
    void Update(double InDelta);
    void UpdateUI();
    
    ECS::EntityID GetStartEntity() const { return StartEntity; }
    Mat4F GetStartOffset() const;

private:
    
    RoomConnectionEditorConfig Config;
    ECS::EntityID EndEntity = ECS::InvalidID;
    ECS::EntityID StartEntity = ECS::InvalidID;

    enum class EditMode
    {
        START,
        END
    } Mode = EditMode::START;
    ECS::EntityID GetCurrent() const; 

};
