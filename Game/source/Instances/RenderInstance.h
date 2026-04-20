#pragma once
#include "Instance/Instance.h"
#include "Resources/Material.h"
#include "Resources/Model.h"

class RenderInstance : public Engine::Instance, public Debug::Panel
{
    CLASS_INFO(RenderInstance, Engine::Instance);
    
public:
    void Init() override;
    void Tick(double InDelta) override;
    void Frame() override;
    
    void DrawPanel() override;
    String PanelName() const override { return "Scene"; }
    bool IsEditor() const override { return true; }

    ResRM skybox = ResRM("Dressing/Skybox/RM_Skybox.json");
    ResRM material = ResRM("Defaults/RM_Default.json");
    Vector<ResModel> sponza;
};
