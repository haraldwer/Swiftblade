#include "Inspector.h"

#ifdef IMGUI_ENABLE

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

#else

void Inspector::DrawDebugPanel() {  }

#endif
