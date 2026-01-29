#include "ImGuiContext.h"

#include "imgui_themes.h"
#include "Commands/Command.h"
#include "Commands/CommandList.h"
#include "Context/Context.h"
#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_wgpu.h"
#include "Window/Window.h"

void Rendering::ImGuiContext::Init(Window &InWindow, Context &InContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGui_ImplGlfw_InitForOther(InWindow.window, true);
    
    ImGui_ImplWGPU_InitInfo info;
    info.Device = InContext.device;
    info.RenderTargetFormat = wgpu::TextureFormat::BGRA8UnormSrgb; // TODO: Use surface
    info.DepthStencilFormat = wgpu::TextureFormat::Undefined; 
    ImGui_ImplWGPU_Init(&info);
    
    ImGui::SetDefaultFlags();
    ImGui::Theme3();
    ImGui::LoadFont(1.0f);
    
    BeginFrame();
}

void Rendering::ImGuiContext::Deinit()
{
    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Rendering::ImGuiContext::BeginFrame()
{
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Rendering::ImGuiContext::EndFrame()
{
    ImGui::EndFrame();
    ImGui::Render();
}

Rendering::Command Rendering::ImGuiContext::Command(RenderTarget &InTarget)
{
    Rendering::Command command("ImGui");
    command.targets = { &InTarget };
    command.customFunc = [&](const wgpu::RenderPassEncoder& renderPass)
    {
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);
    };
    return command;
}
