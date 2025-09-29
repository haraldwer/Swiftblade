#include "Inspector.h"

#include "EditorFunctions.h"

void Inspector::DrawDebugPanel()
{
    EditScene(selected);

    if (ImGui::Begin("Entity"))
        EditHierarchy(selected, selected);
    ImGui::End();

    if (ImGui::Begin("Components"))
        EditComponents(selected);
    ImGui::End();
}
