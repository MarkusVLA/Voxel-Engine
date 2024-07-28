#include "engine/renderer/renderer.h"
#include "engine/window/window.h"

int main() {
    Window window(800, 600, "OpenGL Window");
    Renderer renderer("../src/shaders/triangle.vert", "../src/shaders/triangle.frag");

    while (!window.shouldClose()) {
        window.pollEvents();
        renderer.draw();
        window.swapBuffers();
    }

    return 0;
}
