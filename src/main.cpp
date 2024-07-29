#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/renderer/renderer.h"
#include "engine/window/window.h"
#include "engine/input_listener/input_listener.h"
#include "engine/camera/camera.h"
#include "world/skybox.h"
#include "world/chunk_manager.h"


int main() {
    Window window(1200, 1000, "OpenGL Window");

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    InputListener::setCamera(&camera);

    SkyBox skyBox;
    ChunkManager chunkManager(16, 4, 16, 1); // viewDistance of 2 chunks in each direction

    Renderer renderer;
    renderer.setCamera(&camera);
    renderer.loadTexture("../assets/textures/test.png");
    renderer.setSkyboxData(skyBox.GetVertices());

    // Initial load
    chunkManager.update(camera.getPosition());
    std::vector<float> vertices = chunkManager.getVertexData();
    std::vector<unsigned int> indices = chunkManager.getIndexData();
    renderer.setMeshData(vertices, indices);

    while (!window.shouldClose()) {
        window.pollEvents();

        chunkManager.update(camera.getPosition());

        if (chunkManager.hasChunksChanged()) {
            std::vector<float> vertices = chunkManager.getVertexData();
            std::vector<unsigned int> indices = chunkManager.getIndexData();
            renderer.updateMeshData(vertices, indices);
        }

        renderer.draw();
        window.swapBuffers();
    }

    return 0;
}