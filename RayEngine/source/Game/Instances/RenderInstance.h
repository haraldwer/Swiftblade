#pragma once
#include "Instance/Instance.h"

class RenderInstance : public Engine::Instance, public Debug::Panel
{
    CLASS_INFO(RenderInstance, Instance);
    
public:
    void Init() override;
    void Logic(double InDelta) override;
    void Frame() override;
    
    void DrawPanel() override;
    String PanelName() const override { return "Scene"; }
    bool IsEditor() const override { return true; }

    ResRM skybox = ResRM("Dressing/Skybox/RM_Skybox.json");
    ResRM material = ResRM("Defaults/RM_Default.json");
    struct SponzaModel
    {
        ResModel model; 
        uint64 hash;
    };
    Vector<SponzaModel> sponza;
    
    uint32 persistantID = 0; 
};
