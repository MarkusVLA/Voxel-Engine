#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    unsigned int getID() const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setFloat(const std::string &name, float value) const;
    void setInt(const std::string &name, int value) const;

private:
    unsigned int ID;
    std::string readFile(const std::string& filePath);
    unsigned int createShader(const std::string& source, GLenum shaderType);
    void checkCompileErrors(unsigned int shader, const std::string& type);
};

#endif
