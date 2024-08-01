#ifndef INPUT_LISTENER_H
#define INPUT_LISTENER_H

#include <GLFW/glfw3.h>
#include "../camera/camera.h"

class InputListener {
public:
    static void setCamera(Camera* cam);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
    static Camera* camera;
    static bool firstMouse;
    static float lastX;
    static float lastY;
    static bool isCameraActive;

};

#endif
