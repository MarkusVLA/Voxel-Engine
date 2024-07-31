#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../camera/camera.h"
#include "shader.h"
#include "../../utils/hash.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void initShaders();
    void draw();
    void setViewport(int width, int height);
    void setCamera(Camera* camera);
    void setSkyboxData(const std::vector<float>& vertices);
    void loadTexture(const std::string& path);
    void addChunk(const glm::ivec2& chunkPos, const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void updateChunk(const glm::ivec2& chunkPos, const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void removeChunk(const glm::ivec2& chunkPos);
    void processChunkUpdates();

private:
    struct ChunkMesh {
        GLuint VAO, VBO, EBO;
        size_t indexCount;
    };

    enum class ChunkUpdateType {
        Add,
        Update,
        Remove
    };

    struct ChunkUpdate {
        ChunkUpdateType type;
        glm::ivec2 chunkPos;
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
    };

    std::unordered_map<glm::ivec2, ChunkMesh, IVec2Hash> chunkMeshes;
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int textureID;
    Shader* objectShader;
    Shader* skyboxShader;
    Camera* camera;
    glm::mat4 projection;

    std::queue<ChunkUpdate> chunkUpdateQueue;
    std::mutex queueMutex;
    std::mutex chunkMutex;
    std::mutex textureMutex;
    std::condition_variable queueCV;
    std::atomic<bool> shouldExit;
    std::atomic<bool> textureLoaded;

    void initOpenGL();
    void addChunkImpl(const glm::ivec2& chunkPos, const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void updateChunkImpl(const glm::ivec2& chunkPos, const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void removeChunkImpl(const glm::ivec2& chunkPos);
};

#endif // RENDERER_H