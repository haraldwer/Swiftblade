#include "Input.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

bool Rendering::Input::GetKey(int InKey) const
{
    CHECK_ASSERT(!window, "Invalid window");
    return glfwGetKey(static_cast<GLFWwindow *>(window), InKey) == GLFW_PRESS;
}

bool Rendering::Input::GetMouseButton(int InButton) const
{
    CHECK_ASSERT(!window, "Invalid window");
    return glfwGetMouseButton(static_cast<GLFWwindow *>(window), InButton) == GLFW_PRESS;
}

bool Rendering::Input::GetGamepadButton(int InButton) const
{
    CHECK_ASSERT(!window, "Invalid window");
    GLFWgamepadstate state;
    if (glfwGetGamepadState(0, &state) == GLFW_TRUE)
        return state.buttons[InButton] == GLFW_PRESS; 
    return false;
}

float Rendering::Input::GetGamepadAxis(int InAxis) const
{
    CHECK_ASSERT(!window, "Invalid window");
    GLFWgamepadstate state;
    if (glfwGetGamepadState(0, &state) == GLFW_TRUE)
        return state.axes[InAxis]; 
    return 0.0f;
}

Vec2F Rendering::Input::GetMouseWheel() const
{
    CHECK_ASSERT(!window, "Invalid window");
    
}

Vec2D Rendering::Input::GetMousePosition() const
{
    return mousePosition;
}

Vec2D Rendering::Input::GetMouseDelta() const
{
    return mouseDelta;
}

bool Rendering::Input::IsCursorHidden() const
{
    CHECK_ASSERT(!window, "Invalid window");
    int mode = glfwGetInputMode(static_cast<GLFWwindow *>(window), GLFW_CURSOR);
    return mode == GLFW_CURSOR_DISABLED;
}

void Rendering::Input::HideCursor() const
{
    CHECK_ASSERT(!window, "Invalid window");
    glfwSetInputMode(static_cast<GLFWwindow *>(window), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Rendering::Input::ShowCursor() const
{
    CHECK_ASSERT(!window, "Invalid window");
    glfwSetInputMode(static_cast<GLFWwindow *>(window), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void CursorPosUpdated(GLFWwindow* InWindow, double InX, double InY)
{
    Rendering::Input::Get().OnCursorPosUpdated(InWindow, InX, InY);
}

void Rendering::Input::OnCursorPosUpdated(WindowHandle InWindow, double InX, double InY)
{
    CHECK_ASSERT(window != InWindow, "Invalid window");
    
    Vec2D newPos = Vec2D(InX, InY);
    Vec2D delta = newPos - mousePosition;
    mouseDelta += delta;
    mousePosition = newPos;
}

void Rendering::Input::Init(const WindowHandle InWindow)
{
    CHECK_ASSERT(!InWindow, "Invalid window");
    window = InWindow;
    glfwSetCursorPosCallback(static_cast<GLFWwindow *>(window), &CursorPosUpdated);
}

void Rendering::Input::Frame()
{
    mouseDelta = {};
}

void Rendering::Input::Deinit()
{
    window = nullptr;
}
