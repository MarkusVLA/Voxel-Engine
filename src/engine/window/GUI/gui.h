#ifndef GUI_H
#define GUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>

class GUI {
public:
    GUI(GLFWwindow* window);
    ~GUI();

    void newFrame();
    void render();
    void displayInfo(float fps, const glm::vec3& playerPos, int viewDistance, int loadedChunks);
    glm::vec3 getLightDirection();
    void drawCrosshair(); 

private:
    void displayFPS(float fps);
    void displayPlayerInfo(const glm::vec3& playerPos);
    void displayWorldInfo(int viewDistance, int loadedChunks);
    void displayLightDirectionSlider();

    float azimuth;
    float altitude;
    GLuint crosshairVAO, crosshairVBO;
    GLuint crosshairShaderProgram;
};

#endif // GUI_H