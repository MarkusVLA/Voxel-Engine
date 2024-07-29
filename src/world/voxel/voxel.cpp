#include "voxel.h"

Voxel::Voxel(glm::vec3 position, int type) : position(position), type(type) {}

glm::vec3 Voxel::getPosition() const {
    return position;
}

int Voxel::getType() const {
    return type;
}

std::vector<float> Voxel::getVertexData(const glm::vec3& offset) const {
    std::vector<float> vertices;
    glm::vec3 offsetPosition = position + offset;

    // Add vertices for a cube with texture coordinates and normals
    std::vector<float> cubeVertices = {
        // positions                    // texture coords  // normals
        // Front face
        offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,

        // Back face
        offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
        offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
        offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
        offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,

        // Left face
        offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,

        // Right face
        offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

        // Top face
        offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
        offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,

        // Bottom face
        offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
        offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
        offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
        offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f
    };

    vertices.insert(vertices.end(), cubeVertices.begin(), cubeVertices.end());

    return vertices;
}

std::vector<unsigned int> Voxel::getIndexData(unsigned int baseIndex) const {
    std::vector<unsigned int> indices;

    std::vector<unsigned int> cubeIndices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20
    };

    for (auto idx : cubeIndices) {
        indices.push_back(idx + baseIndex);
    }

    return indices;
}
