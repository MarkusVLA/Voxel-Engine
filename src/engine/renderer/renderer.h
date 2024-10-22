// renderer.h
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
    void addChunk(const glm::ivec2& chunkPos, const std::vector<float>& solidVertices, const std::vector<unsigned int>& solidIndices,
                  const std::vector<float>& waterVertices, const std::vector<unsigned int>& waterIndices);
    void updateChunk(const glm::ivec2& chunkPos, const std::vector<float>& solidVertices, const std::vector<unsigned int>& solidIndices,
                     const std::vector<float>& waterVertices, const std::vector<unsigned int>& waterIndices);
    void removeChunk(const glm::ivec2& chunkPos);
    void processChunkUpdates();
    void setLightDir(glm::vec3 dir);

private:
    enum class ChunkUpdateType {
        Add,
        Update,
        Remove
    };

    struct ChunkMesh {
        GLuint solidVAO, solidVBO, solidEBO;
        GLuint waterVAO, waterVBO, waterEBO;
        size_t solidIndexCount;
        size_t waterIndexCount;
    };

    struct ChunkUpdate {
        ChunkUpdateType type;
        glm::ivec2 chunkPos;
        std::vector<float> solidVertices;
        std::vector<unsigned int> solidIndices;
        std::vector<float> waterVertices;
        std::vector<unsigned int> waterIndices;
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
    glm::vec3 lightDir;

    void initOpenGL();
    void addChunkImpl(const glm::ivec2& chunkPos, const std::vector<float>& solidVertices, const std::vector<unsigned int>& solidIndices,
                      const std::vector<float>& waterVertices, const std::vector<unsigned int>& waterIndices);
    void updateChunkImpl(const glm::ivec2& chunkPos, const std::vector<float>& solidVertices, const std::vector<unsigned int>& solidIndices,
                         const std::vector<float>& waterVertices, const std::vector<unsigned int>& waterIndices);
    void removeChunkImpl(const glm::ivec2& chunkPos);
    void setupShaderUniforms(Shader* shader, const glm::mat4& view, glm::mat4& projection);
    void renderChunk(Shader* shader, GLuint vao, int indexCount, const glm::ivec2& chunkPos);

};

#endif // RENDERER_H