#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/renderer/renderer.h"
#include "engine/window/window.h"
#include "engine/input_listener/input_listener.h"
#include "engine/camera/camera.h"
#include "world/chunk.h"
#include "world/skybox.h"


int main() {
    Window window(1200, 1000, "OpenGL Window");

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    InputListener::setCamera(&camera);
    SkyBox SkyBox;
    Chunk chunk(128, 4, 128); 

    std::vector<float> vertices = chunk.getVertexData();
    std::vector<unsigned int> indices = chunk.getIndexData();
    Renderer renderer;

    renderer.setMeshData(vertices, indices);
    renderer.setCamera(&camera);
    renderer.loadTexture("../assets/textures/test.png");
    renderer.setSkyboxData(SkyBox.GetVertices());


    while (!window.shouldClose()) {
        window.pollEvents();
        renderer.draw();
        window.swapBuffers();
    }

    return 0;
}