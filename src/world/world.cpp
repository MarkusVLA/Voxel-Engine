#include "world.h"
#include <cstdlib>
#include <ctime>

World::World(int width, int height, int depth) : width(width), height(height), depth(depth) {
    std::srand(static_cast<unsigned>(std::time(0)));
    generateTerrain();
}

void World::generateTerrain() {
    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            int y = generateHeight(x, z);
            for (int h = 0; h < y; ++h) {
                voxels.emplace_back(glm::vec3(x, h, z), 1); // Type 1 for terrain blocks
            }
        }
    }
}

int World::generateHeight(int x, int z) const {
    // Simple height generation using sine waves (for demonstration purposes)
    float scale = 0.1f;
    return static_cast<int>((std::sin(x * scale) + std::cos(z * scale)) * 5 + 10);
}

std::vector<float> World::getVertexData() const {
    std::vector<float> vertices;

    for (const auto& voxel : voxels) {
        glm::vec3 pos = voxel.getPosition();

        // Add vertices for a cube with texture coordinates and normals
        std::vector<float> cubeVertices = {
            // positions           // texture coords  // normals
            // Front face
            pos.x - 0.5f, pos.y - 0.5f, pos.z + 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
            pos.x + 0.5f, pos.y - 0.5f, pos.z + 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
            pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
            pos.x - 0.5f, pos.y + 0.5f, pos.z + 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,

            // Back face
            pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
            pos.x + 0.5f, pos.y - 0.5f, pos.z - 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
            pos.x + 0.5f, pos.y + 0.5f, pos.z - 0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
            pos.x - 0.5f, pos.y + 0.5f, pos.z - 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,

            // Left face
            pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
            pos.x - 0.5f, pos.y - 0.5f, pos.z + 0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
            pos.x - 0.5f, pos.y + 0.5f, pos.z + 0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
            pos.x - 0.5f, pos.y + 0.5f, pos.z - 0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,

            // Right face
            pos.x + 0.5f, pos.y - 0.5f, pos.z - 0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
            pos.x + 0.5f, pos.y - 0.5f, pos.z + 0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
            pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
            pos.x + 0.5f, pos.y + 0.5f, pos.z - 0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

            // Top face
            pos.x - 0.5f, pos.y + 0.5f, pos.z - 0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
            pos.x + 0.5f, pos.y + 0.5f, pos.z - 0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
            pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
            pos.x - 0.5f, pos.y + 0.5f, pos.z + 0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,

            // Bottom face
            pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
            pos.x + 0.5f, pos.y - 0.5f, pos.z - 0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
            pos.x + 0.5f, pos.y - 0.5f, pos.z + 0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
            pos.x - 0.5f, pos.y - 0.5f, pos.z + 0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f
        };

        vertices.insert(vertices.end(), cubeVertices.begin(), cubeVertices.end());
    }

    return vertices;
}



std::vector<unsigned int> World::getIndexData() const {
    std::vector<unsigned int> indices;
    unsigned int index = 0;

    for (size_t i = 0; i < voxels.size(); ++i) {
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
            indices.push_back(idx + index);
        }

        index += 24; // Each cube adds 24 vertices (6 faces * 4 vertices per face)
    }

    return indices;
}
