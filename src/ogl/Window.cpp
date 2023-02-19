// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#include <glm/gtc/matrix_transform.hpp>
#include "Window.h"
#include "Game.h"
#include "Log.h"
#include "MdciiException.h"
#include "event/EventManager.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::ogl::Window::Window()
{
    Log::MDCII_LOG_DEBUG("[Window::Window()] Create Window.");

    Init();
}

mdcii::ogl::Window::~Window() noexcept
{
    Log::MDCII_LOG_DEBUG("[Window::~Window()] Destruct Window.");

    CleanUp();
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const std::string& mdcii::ogl::Window::GetTitle() const
{
    return m_title;
}

const glm::mat4& mdcii::ogl::Window::GetOrthographicProjectionMatrix() const
{
    return m_orthographicProjectionMatrix;
}

GLFWwindow* mdcii::ogl::Window::GetWindowHandle() const
{
    return m_windowHandle;
}

//-------------------------------------------------
// Rendering
//-------------------------------------------------

void mdcii::ogl::Window::SwapBuffersAndCallEvents() const
{
    glfwSwapBuffers(m_windowHandle);
    glfwPollEvents();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

bool mdcii::ogl::Window::WindowShouldClose() const
{
    return glfwWindowShouldClose(m_windowHandle);
}

void mdcii::ogl::Window::Close() const
{
    glfwSetWindowShouldClose(m_windowHandle, true);
}

//-------------------------------------------------
// Input Polling
//-------------------------------------------------

bool mdcii::ogl::Window::IsKeyPressed(const int t_keyCode) const
{
    return glfwGetKey(m_windowHandle, t_keyCode) == GLFW_PRESS ||
        glfwGetKey(m_windowHandle, t_keyCode) == GLFW_REPEAT;
}

bool mdcii::ogl::Window::IsMouseButtonPressed(const int t_button) const
{
    return glfwGetMouseButton(m_windowHandle, t_button) == GLFW_PRESS;
}

glm::vec2 mdcii::ogl::Window::GetMousePosition() const
{
    double x;
    double y;
    glfwGetCursorPos(m_windowHandle, &x, &y);
    return { static_cast<float>(x), static_cast<float>(y) };
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::ogl::Window::ImGuiBegin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void mdcii::ogl::Window::ImGuiEnd()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::ogl::Window::Init()
{
    Log::MDCII_LOG_DEBUG("[Window::Init()] Initializing window.");

    LoadConfig();
    InitWindow();
    InitProjectionMatrix();
    InitImGui();
    InitInputCallbacks();

    Log::MDCII_LOG_DEBUG("[Window::Init()] The window was successfully initialized.");
}

void mdcii::ogl::Window::LoadConfig()
{
    Log::MDCII_LOG_DEBUG("[Window::LoadConfig()] Load values from config file.");

    width = Game::INI.Get<int>("window", "width");
    height = Game::INI.Get<int>("window", "height");

    width = std::max(width, MIN_WIDTH);
    height = std::max(height, MIN_HEIGHT);

    Log::MDCII_LOG_INFO("Game window width: {}", width);
    Log::MDCII_LOG_INFO("Game window height: {}", height);
}

void mdcii::ogl::Window::InitWindow()
{
    Log::MDCII_LOG_DEBUG("[Window::InitWindow()] Initializing GLFW.");

    // Setup an error callback.
    glfwSetErrorCallback(
        [](const int t_error, const char* t_description) {
            Log::MDCII_LOG_ERROR("GLFW Error ({}) {}", t_error, t_description);
        }
    );

    // Initialize GLFW.
    if (!glfwInit())
    {
        throw MDCII_EXCEPTION("[Window::InitWindow()] Unable to initialize GLFW.");
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create the GLFW window.
    m_windowHandle = glfwCreateWindow(width, height, m_title.c_str(), nullptr, nullptr);
    if (!m_windowHandle)
    {
        glfwTerminate();
        throw MDCII_EXCEPTION("[Window::InitWindow()] Failed to create the GLFW window.");
    }

    // Get the resolution of the primary monitor.
    const auto* primaryMonitor{ glfwGetVideoMode(glfwGetPrimaryMonitor()) };
    if (!primaryMonitor)
    {
        throw MDCII_EXCEPTION("[Window::InitWindow()] Unable to get the primary monitor.");
    }

    // Center our window.
    glfwSetWindowPos(GetWindowHandle(), (primaryMonitor->width - width) / 2, (primaryMonitor->height - height) / 2);

    // Make the OpenGL context current.
    glfwMakeContextCurrent(GetWindowHandle());

    // Update viewport.
    glViewport(0, 0, width, height);

    // Initialize GLEW.
    Log::MDCII_LOG_DEBUG("[Window::InitWindow()] Initializing GLEW.");
    if (const auto err{ glewInit() }; err != GLEW_OK)
    {
        throw MDCII_EXCEPTION("[Window::InitWindow()] Unable to initialize GLEW." + std::string(reinterpret_cast<const char*>(glewGetErrorString(err))));
    }

    // Print out some information about the graphics drivers.
    Log::MDCII_LOG_INFO("OpenGL version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    Log::MDCII_LOG_INFO("GLSL version: {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    Log::MDCII_LOG_INFO("Vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    Log::MDCII_LOG_INFO("Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    // Make the window visible.
    glfwShowWindow(GetWindowHandle());
}

void mdcii::ogl::Window::InitProjectionMatrix()
{
    Log::MDCII_LOG_DEBUG("[Window::InitProjectionMatrix()] Initializing projection.");

    UpdateOrthographicProjectionMatrix();
}

void mdcii::ogl::Window::InitImGui() const
{
    Log::MDCII_LOG_DEBUG("[Window::InitImGui()] Initializing ImGui.");

    // setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // setup renderer bindings without callbacks
    // the callbacks from ImGui will later be integrated into our own
    ImGui_ImplGlfw_InitForOpenGL(m_windowHandle, false);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // setup fonts
    ImGuiIO& io = ImGui::GetIO();
    auto fontPath{ Game::RESOURCES_REL_PATH + "font/DejaVuSansMono.ttf" };
    static ImWchar ranges[] = { 0x20, 0xFFFF, 0 };
    io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 12, nullptr, ranges);

    // setup style
    ImGui::StyleColorsDark();
}

void mdcii::ogl::Window::InitInputCallbacks() const
{
    Log::MDCII_LOG_DEBUG("[Window::InitInputCallbacks()] Initializing input callbacks.");

    event::EventManager::SetKeyboardGlfwCallbacks(m_windowHandle);
    event::EventManager::SetMouseGlfwCallbacks(m_windowHandle);
}

//-------------------------------------------------
// Projection matrix
//-------------------------------------------------

void mdcii::ogl::Window::UpdateOrthographicProjectionMatrix()
{
    m_orthographicProjectionMatrix = glm::ortho(
        0.0f,
        static_cast<float>(width),
        static_cast<float>(height),
        0.0f,
        -1.0f,
        1.0f
    );
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::ogl::Window::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[Window::CleanUp()] CleanUp window.");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_windowHandle);
    glfwTerminate();
}
