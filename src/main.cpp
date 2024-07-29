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

    int worldWidth = 8;
    int worldHeight = 8; 
    ChunkManager chunkManager(16, 32, 16, worldWidth, worldHeight);

    Renderer renderer;
    renderer.setCamera(&camera);
    renderer.loadTexture("../assets/textures/atlas.png");
    renderer.setSkyboxData(skyBox.GetVertices());

    std::vector<float> vertices = chunkManager.getVertexData();
    std::vector<unsigned int> indices = chunkManager.getIndexData();
    renderer.setMeshData(vertices, indices);

    while (!window.shouldClose()) {
        window.pollEvents();
        renderer.draw();
        window.swapBuffers();
    }

    return 0;
}
