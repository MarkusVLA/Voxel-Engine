#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../camera/camera.h"
#include "shader.h"
#include "../../utils/hash.h"


class Renderer {
public:
    Renderer();
    ~Renderer();
    void draw();
    void setViewport(int width, int height);
    void setCamera(Camera* camera);
    void setSkyboxData(const std::vector<float>& vertices);
    void loadTexture(const std::string& path);
    void addChunk(const glm::ivec2& chunkPos, const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void updateChunk(const glm::ivec2& chunkPos, const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void removeChunk(const glm::ivec2& chunkPos);

private:
    struct ChunkMesh {
        GLuint VAO, VBO, EBO;
        size_t indexCount;
    };

    std::unordered_map<glm::ivec2, ChunkMesh, IVec2Hash> chunkMeshes; 
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int textureID;
    Shader* objectShader;
    Shader* skyboxShader;
    Camera* camera;
    glm::mat4 projection;
    void initOpenGL();
};

#endif // RENDERER_H