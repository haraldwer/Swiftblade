#include "ImGuizmo_Wrapper.h"

#include "ImGui/imgui.h"
#include "ImGui/Gizmo/ImGuizmo.h"
#include "Instance/Instance.h"
#include "Rendering/Scene/Instances/CameraInstance.h"
#include "Core/Utility/RayUtility.h"
#include "raylib.h"
#include <rcamera.h>

#include "Rendering/Manager.h"

bool ImGuizmo::Edit(Mat4F& InOutMat, int& InOutSpace, int& InOutOperation, bool& InOutUseSnap)
{
    // Get camera
    const Rendering::CameraInstance camInstance = Engine::Instance::Get().GetRenderScene().GetCamera();
    Vec2F size = Rendering::Manager::Get().window.GetSize().To<float>();
    Mat4F proj = Mat4F::Transpose(camInstance.GetProjectionMatrix(size));
    Mat4F view = Mat4F::Transpose(camInstance.GetViewMatrix());

    // Mode / operation UI
    if (InOutOperation == 0)
        InOutOperation = TRANSLATE;
    if (InOutSpace == 0)
        InOutSpace = LOCAL; 
    if (ImGui::RadioButton("Translate", InOutOperation == TRANSLATE))
        InOutOperation = TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", InOutOperation == ROTATE))
        InOutOperation = ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", InOutOperation == SCALE))
        InOutOperation = SCALE;
    if (ImGui::RadioButton("Local", InOutSpace == LOCAL))
        InOutSpace = LOCAL;
    ImGui::SameLine();
    if (ImGui::RadioButton("World", InOutSpace == WORLD))
        InOutSpace = WORLD;

    // Snapping
    ImVec4 snap = { 0,0,0,0 };
    static ImVec4 snapTranslation = { 0.1f , 0.1f, 0.1f, 0.1f };
    static float snapAngle = 5.0f;
    static float snapScale = 0.1f;
    ImGui::Checkbox("##UseSnap", &InOutUseSnap);
    ImGui::SameLine();
    switch (InOutOperation)
    {
    case TRANSLATE:
        ImGui::InputFloat3("Snap", &snapTranslation.x);
        snap = snapTranslation;
        break;
    case ROTATE:
        ImGui::InputFloat("Angle Snap", &snapAngle);
        snap = { snapAngle, snapAngle, snapAngle, snapAngle };
        break;
    case SCALE:
        ImGui::InputFloat("Scale Snap", &snapScale);
        snap = { snapScale, snapScale, snapScale, snapScale };
        break;
    }

    const ImGuiIO& io = ImGui::GetIO();
    SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    
    const bool result = Manipulate(
        view.data,
        proj.data,
        static_cast<OPERATION>(InOutOperation),
        static_cast<MODE>(InOutSpace),
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
