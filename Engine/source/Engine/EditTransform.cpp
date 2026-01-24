#include "EditTransform.h"

#include "ImGui/imgui.h"
#include "ImGui/Gizmo/ImGuizmo.h"
#include "Instance/Instance.h"
#include "Rendering/Scene/Instances/CameraInstance.h"

#include "Rendering/Manager.h"

bool Engine::EditTransform(Mat4F &InOutMat, int &InOutSpace, int &InOutOperation, bool &InOutUseSnap)
{
    // Mode / operation UI
    if (InOutOperation == 0)
        InOutOperation = ImGuizmo::TRANSLATE;
    if (InOutSpace == 0)
        InOutSpace = ImGuizmo::LOCAL; 
    if (ImGui::RadioButton("Translate", InOutOperation == ImGuizmo::TRANSLATE))
        InOutOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", InOutOperation == ImGuizmo::ROTATE))
        InOutOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", InOutOperation == ImGuizmo::SCALE))
        InOutOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Local", InOutSpace == ImGuizmo::LOCAL))
        InOutSpace = ImGuizmo::LOCAL;
    ImGui::SameLine();
    if (ImGui::RadioButton("World", InOutSpace == ImGuizmo::WORLD))
        InOutSpace = ImGuizmo::WORLD;

    // Snapping
    ImVec4 snap = { 0,0,0,0 };
    static ImVec4 snapTranslation = { 0.1f , 0.1f, 0.1f, 0.1f };
    static float snapAngle = 5.0f;
    static float snapScale = 0.1f;
    ImGui::Checkbox("##UseSnap", &InOutUseSnap);
    ImGui::SameLine();
    switch (InOutOperation)
    {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snapTranslation.x);
            snap = snapTranslation;
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snapAngle);
            snap = { snapAngle, snapAngle, snapAngle, snapAngle };
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snapScale);
            snap = { snapScale, snapScale, snapScale, snapScale };
            break;
    }
        
    // Set viewport
    auto& viewport = Rendering::Manager::Get().mainViewport;
    Vec2F pos = viewport.GetPosition();
    Vec2F size = viewport.GetResolution().To<float>();
    ImVec2 offset = ImGui::GetMainViewport()->Pos;
    ImGuizmo::SetRect(pos.x - offset.x, pos.y - offset.y, size.x, size.y);
        
    // Get camera
    const Rendering::CameraInstance camInstance = Engine::Instance::Get().GetRenderScene().GetCamera();
    Mat4F proj = camInstance.GetProjectionMatrix(size);
    Mat4F view = Mat4F::GetInverse(camInstance.GetViewMatrix());
    
    const bool result = ImGuizmo::Manipulate(
        view.data,
        proj.data,
        static_cast<ImGuizmo::OPERATION>(InOutOperation),
        static_cast<ImGuizmo::MODE>(InOutSpace),
        InOutMat.data,
        nullptr,
        InOutUseSnap ? &snap.x : nullptr);
        
    static bool edited;
    if (result)
        edited = true;
        
    if (edited && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        edited = false;
        return true; 
    }
        
    return false; 
}
