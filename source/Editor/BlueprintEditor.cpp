#include "BlueprintEditor.h"

#include "raylib/raygui.h"

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Rendering/Renderer.h"
#include "Game/Game.h"

void BlueprintEditor::Init()
{
    Instance::Init();
    SetBP(ResBlueprint("player.json"));
}

void BlueprintEditor::SetBP(const ResBlueprint& InBP)
{
    Blueprint = InBP;
    if (BlueprintResource* bp = Blueprint.Get())
        InstanceID = bp->Instantiate();
}

void BlueprintEditor::Update(double InDelta)
{
    Time.Frame(InDelta);
    
    // Update
    ECS.Update(Time.Delta());
    Camera.Update(InDelta);
    
    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_S))
            if (BlueprintResource* bp = Blueprint.Get())
                bp->Serialize(InstanceID); 
        
        if (IsKeyPressed(KEY_P))
            Engine::Manager::Get().Push<Game>();
    }
}

void BlueprintEditor::UpdateUI()
{
    return;
    GuiPanel(Rectangle(30, 30, 100, 100), "Panel");
    
    if (GuiButton(Rectangle(10, 10, 100, 20), "Test"))
    {
        TraceLog(LOG_INFO, "Test!");
    }
    GuiStatusBar(Rectangle(0, 0, GetScreenWidth(), 10), "Status bar");
}
