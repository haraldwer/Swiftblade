#pragma once
#include "EditorCamera.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Instance.h"

class BlueprintEditor : public Engine::Instance
{
public:

    void Init() override;
    void Update(double InDelta) override;
    void DrawUI() override;
    bool IsEditor() const override { return true; }
    
    void SetBP(const ResBlueprint& InBP);

private:

    void EditComponents(ECS::EntityID InID);
    void EditHierarhcy(ECS::EntityID InID);
    void HierarchyNode(ECS::EntityID InID);

    String Text(const String& InString) const;
    
    EditorCamera Camera;
    ResBlueprint Blueprint;
    ECS::EntityID InstanceID = ECS::InvalidID; 
    ECS::EntityID SelectedID = ECS::InvalidID;  
    
};
