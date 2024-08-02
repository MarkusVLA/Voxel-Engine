#define GL_SILENCE_DEPRECATION
#include "gui.h"
#include <sstream>
#include <iomanip>
#include <glm/trigonometric.hpp>

GUI::GUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    azimuth = 45.0f;
    altitude = 20.0f;

    float crosshairVertices[] = {
        -0.01f, 0.0f, 0.0f,
         0.01f, 0.0f, 0.0f,
         0.0f, -0.016f, 0.0f,
         0.0f,  0.016f, 0.0f
    };

    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Crosshair shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        }
    )";
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    crosshairShaderProgram = glCreateProgram();
    glAttachShader(crosshairShaderProgram, vertexShader);
    glAttachShader(crosshairShaderProgram, fragmentShader);
    glLinkProgram(crosshairShaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

GUI::~GUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glDeleteVertexArrays(1, &crosshairVAO);
    glDeleteBuffers(1, &crosshairVBO);
    glDeleteProgram(crosshairShaderProgram);
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
    ImGui::Separator();
    displayLightDirectionSlider();

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

void GUI::displayLightDirectionSlider() {
    ImGui::Text("Light Direction");
    ImGui::SliderFloat("Azimuth", &azimuth, 0.0f, 360.0f);
    ImGui::SliderFloat("Altitude", &altitude, -90.0f, 90.0f);
}

glm::vec3 GUI::getLightDirection() {
    // Convert spherical coordinates to Cartesian coordinates
    float azimuthRad = glm::radians(azimuth);
    float altitudeRad = glm::radians(altitude);
    
    float x = glm::cos(altitudeRad) * glm::sin(azimuthRad);
    float y = glm::sin(altitudeRad);
    float z = glm::cos(altitudeRad) * glm::cos(azimuthRad);

    return glm::normalize(glm::vec3(x, y, z));
}

void GUI::drawCrosshair() {
    glUseProgram(crosshairShaderProgram);
    glBindVertexArray(crosshairVAO);
    glDrawArrays(GL_LINES, 0, 4);
}