#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../camera/camera.h"

class Renderer {
public:
    Renderer(const std::string& vertexPath, const std::string& fragmentPath);
    ~Renderer();
    void draw();
    void setViewport(int width, int height);
    void setCamera(Camera* camera);
    void setMeshData(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;
    Camera* camera;
    glm::mat4 projection;
    std::string readFile(const std::string& filePath);
    unsigned int createShader(const std::string& source, GLenum shaderType);
    void checkCompileErrors(unsigned int shader, const std::string& type);
    void initOpenGL(const std::string& vertexPath, const std::string& fragmentPath);

    size_t indicesSize; // Store the size of the indices
};

#endif
