// gui.h
#ifndef GUI_H
#define GUI_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class GUI {
public:
    GUI(GLFWwindow* window);
    ~GUI();

    void newFrame();
    void render();
    void displayInfo(float fps, const glm::vec3& playerPos, int viewDistance, int loadedChunks);

private:
    void displayFPS(float fps);
    void displayPlayerInfo(const glm::vec3& playerPos);
    void displayWorldInfo(int viewDistance, int loadedChunks);
};

#endif // GUI_H