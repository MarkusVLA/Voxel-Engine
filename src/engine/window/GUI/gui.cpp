// gui.cpp
#include "gui.h"

GUI::GUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

GUI::~GUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::displayFPS(float fps) {
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | 
                 ImGuiWindowFlags_NoSavedSettings);
    ImGui::Text("FPS: %.1f", fps);
    ImGui::End();
}