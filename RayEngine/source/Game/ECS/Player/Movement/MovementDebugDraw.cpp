#include "Engine/ECS/Systems/Rigidbody.h"
#include "ImGui/imgui.h"
#include "Movement.h"

bool ECS::Movement::EditState() const
{
    const Rigidbody& rb = GetRB();
    const Vec3F vel = rb.GetVelocity();
        
    ImGui::Text(("Speed: " + std::to_string(vel.Length())).c_str());
    ImGui::Text(("Velocity: x" + std::to_string(vel.x) + " y" + std::to_string(vel.y) + " z" + std::to_string(vel.z)).c_str());
    ImGui::Spacing();
    ImGui::Text(("InAir: " + std::to_string(IsInAir())).c_str()); 
    ImGui::Text(("OnGround: " + std::to_string(IsOnGround())).c_str()); 
    ImGui::Text(("Crouching: " + std::to_string(IsCrouching())).c_str()); 
    ImGui::Spacing();

    if (!StateMachine)
        return false;
    if (ImGui::Button("Save##Movement"))
        StateMachine->SaveConfig();
    return StateMachine->Edit();
    
}
