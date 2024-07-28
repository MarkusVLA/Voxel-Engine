#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <glad/glad.h>

class Renderer {
public:
    Renderer(const std::string& vertexPath, const std::string& fragmentPath);
    ~Renderer();
    void draw();
    void setViewport(int width, int height);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;
    std::string readFile(const std::string& filePath);
    unsigned int createShader(const std::string& source, GLenum shaderType);
    void checkCompileErrors(unsigned int shader, const std::string& type);
    void initOpenGL(const std::string& vertexPath, const std::string& fragmentPath);
};

#endif
