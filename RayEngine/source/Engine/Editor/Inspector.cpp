#include "Inspector.h"

#include "EditorFunctions.h"

void Inspector::DrawDebugPanel()
{
    auto prevSceneSelected = selected;
    EditScene(selected);
    if (selected != prevSceneSelected)
        hierarchySelected = selected;

    if (ImGui::Begin("Entity"))
        EditHierarchy(selected, hierarchySelected);
    ImGui::End();

    if (ImGui::Begin("Components"))
        EditComponents(hierarchySelected);
    ImGui::End();
}
