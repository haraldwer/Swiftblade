#include "BlueprintResourceDetails.h"

#include "BlueprintEditor.h"
#include "Instance/Manager.h"

void Editor::BlueprintResourceDetails::Init()
{
    resource = path;
    content = Utility::File::Read(path);

    auto top = Engine::Manager::Get().Top();
    if (top->IsA(Type::Get<BlueprintEditor>()))
    {
        Utility::Cast<BlueprintEditor>(top)->SetBP(resource);
    }
    else
    {
        BlueprintEditor* editor = Engine::Manager::Get().Push<BlueprintEditor>();
        editor->SetPendingBP(resource);
    }
}

void Editor::BlueprintResourceDetails::Deinit()
{
    // TODO: Maybe close?
}

void Editor::BlueprintResourceDetails::Draw()
{
    DrawFileInfo();
    
    if (RawTextEdit())
    {
        resource.Reload();
        auto top = Engine::Manager::Get().Top();
        if (top->IsA(Type::Get<BlueprintEditor>()))
            Utility::Cast<BlueprintEditor>(top)->SetBP(resource);
    }
}

bool Editor::BlueprintResourceDetails::Accept(const String &InPath)
{
    return BlueprintResource::Accept(InPath);
}
