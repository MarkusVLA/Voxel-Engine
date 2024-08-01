// gui.h
#ifndef GUI_H
#define GUI_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

class GUI {
public:
    GUI(GLFWwindow* window);
    ~GUI();

    void newFrame();
    void render();
    void displayFPS(float fps);

private:
    float fps;
};

#endif // GUI_H