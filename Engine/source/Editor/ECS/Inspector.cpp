#include "Inspector.h"

#include "ImGui/imgui.h"
#include "../Utility/EditorFunctions.h"

void Editor::Inspector::DrawPanel()
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
