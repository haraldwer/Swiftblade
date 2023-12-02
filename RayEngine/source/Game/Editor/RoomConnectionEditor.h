#pragma once
#include "RoomSubEditor.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"

struct RoomConnectionEditorConfig : PropertyOwner<RoomConnectionEditorConfig>
{
    PROPERTY_C(ResBlueprint, EndBP, "Blueprints/BP_RoomConnection.json");
    
    inline static const String Path = "Configs/C_RoomConnectionEditor.json";
    void LoadConfig() { Load(Path); }
    void SaveConfig() const { Save(Path); }
};

class RoomConnectionEditor : public RoomSubEditor
{
public:
    void Init() override;
    void Deinit() override;
    void Update(double InDelta) override;
    void UpdateUI(bool InIsCameraControlling) override;
    
    Mat4F GetStartOffset() const;

private:
    
    RoomConnectionEditorConfig Config;
    ECS::EntityID EndEntity = ECS::InvalidID;
    ECS::EntityID StartEntity = ECS::InvalidID;
    Mat4F EditStartTrans; 

    enum class EditMode
    {
        START,
        END
    } Mode = EditMode::START;
    
    ECS::EntityID GetCurrent() const;
    static Mat4F GetTrans(ECS::EntityID InID);
    void SetMode(EditMode InMode);     

};
