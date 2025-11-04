#include "Engine/ECS/Systems/Rigidbody.h"
#include "ImGui/imgui.h"
#include "Movement.h"
#include "MovementStateMachine.h"

#ifdef IMGUI_ENABLE

bool ECS::Movement::EditState() const
{
    const Rigidbody& rb = GetRB();
    const Vec3F vel = rb.GetVelocity();
        
    ImGui::Text("Speed: %f", vel.Length());
    ImGui::Text("Velocity: %s", Utility::ToStr(vel).c_str());
    ImGui::Spacing();
    ImGui::Text("InAir: %i", IsInAir()); 
    ImGui::Text("OnGround: %i", IsOnGround()); 
    ImGui::Text("Crouching: %i", IsCrouching()); 
    ImGui::Spacing();

    if (!stateMachine)
        return false;
    if (ImGui::Button("Save##Movement"))
        stateMachine->SaveConfig();
    return stateMachine->Edit();
    
}

#else

bool ECS::Movement::EditState() const { return false; }

#endif
