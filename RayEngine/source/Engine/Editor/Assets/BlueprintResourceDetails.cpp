#include "BlueprintResourceDetails.h"

#include "Editor/BlueprintEditor.h"
#include "Instance/Manager.h"

void BlueprintResourceDetails::Init()
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

void BlueprintResourceDetails::Deinit()
{
    // TODO: Maybe close?
}

void BlueprintResourceDetails::Draw()
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

bool BlueprintResourceDetails::Accept(const String &InPath)
{
    return BlueprintResource::Accept(InPath);
}
