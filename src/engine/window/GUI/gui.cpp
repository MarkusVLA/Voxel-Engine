// gui.cpp
#include "gui.h"
#include <sstream>
#include <iomanip>

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

void GUI::displayInfo(float fps, const glm::vec3& playerPos, int viewDistance, int loadedChunks) {
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Game Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | 
                 ImGuiWindowFlags_NoSavedSettings);

    displayFPS(fps);
    ImGui::Separator();
    displayPlayerInfo(playerPos);
    ImGui::Separator();
    displayWorldInfo(viewDistance, loadedChunks);

    ImGui::End();
}

void GUI::displayFPS(float fps) {
    ImGui::Text("FPS: %.1f", fps);
}

void GUI::displayPlayerInfo(const glm::vec3& playerPos) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Player Position: (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << ")";
    ImGui::Text("%s", oss.str().c_str());
}

void GUI::displayWorldInfo(int viewDistance, int loadedChunks) {
    ImGui::Text("View Distance: %d chunks", viewDistance);
    ImGui::Text("Loaded Chunks: %d", loadedChunks);
}