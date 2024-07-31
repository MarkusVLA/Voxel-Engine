#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../camera/camera.h"
#include "shader.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void draw();
    void setViewport(int width, int height);
    void setCamera(Camera* camera);
    void setMeshData(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void setSkyboxData(const std::vector<float>& vertices);
    void loadTexture(const std::string& path);
    void updateMeshData(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int depthVAO, dpehtVBO;
    unsigned int textureID;
    Shader* objectShader;
    Shader* skyboxShader;
    Camera* camera;
    glm::mat4 projection;
    size_t indicesSize;
    void initOpenGL();
};

#endif // RENDERER_H
