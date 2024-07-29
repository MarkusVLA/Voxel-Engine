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

    SkyBox skyBox;
    Chunk chunk(16, 4, 16, {2, 2});
    Chunk chunk2(16, 4, 16, {0, 0});

    std::vector<float> vertices1 = chunk.getVertexData();
    std::vector<unsigned int> indices1 = chunk.getIndexData();

    std::vector<float> vertices2 = chunk2.getVertexData();
    std::vector<unsigned int> indices2 = chunk2.getIndexData();

    unsigned int offsetIndex = vertices1.size() / 8; // 8 floats per vertex

    vertices1.insert(vertices1.end(), vertices2.begin(), vertices2.end());

    for (unsigned int index : indices2) {
        indices1.push_back(index + offsetIndex);
    }

    Renderer renderer;

    renderer.setMeshData(vertices1, indices1);
    renderer.setCamera(&camera);
    renderer.loadTexture("../assets/textures/test.png");
    renderer.setSkyboxData(skyBox.GetVertices());

    while (!window.shouldClose()) {
        window.pollEvents();
        renderer.draw();
        window.swapBuffers();
    }

    return 0;
}
