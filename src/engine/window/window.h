#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <GLFW/glfw3.h>
#include "../input_listener/input_listener.h"


class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();
    bool shouldClose();
    void swapBuffers();
    void pollEvents();
    GLFWwindow* getGLFWwindow();

    void toggleCursorMode();
    bool isCursorEnabled() const;


private:
    GLFWwindow* window;
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    bool cursorEnabled;

};

#endif
