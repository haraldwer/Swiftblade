#pragma once
#include "EditorCamera.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Instance.h"

class BlueprintEditor : public Engine::Instance
{
public:

    void Init() override;
    void Update(double InDelta) override;
    void UpdateUI() override;
    bool IsEditor() const override { return true; }
    
    void SetBP(const ResBlueprint& InBP);

private:

    EditorCamera Camera;
    ResBlueprint Blueprint;
    ECS::EntityID InstanceID = ECS::InvalidID; 
    
};
