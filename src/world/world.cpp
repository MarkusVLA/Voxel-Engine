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

        // Add vertices for a cube
        std::vector<glm::vec3> cubeVertices = {
            {pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f},
            {pos.x + 0.5f, pos.y - 0.5f, pos.z - 0.5f},
            {pos.x + 0.5f, pos.y + 0.5f, pos.z - 0.5f},
            {pos.x - 0.5f, pos.y + 0.5f, pos.z - 0.5f},
            {pos.x - 0.5f, pos.y - 0.5f, pos.z + 0.5f},
            {pos.x + 0.5f, pos.y - 0.5f, pos.z + 0.5f},
            {pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f},
            {pos.x - 0.5f, pos.y + 0.5f, pos.z + 0.5f}
        };

        for (const auto& v : cubeVertices) {
            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);
        }
    }

    return vertices;
}

std::vector<unsigned int> World::getIndexData() const {
    std::vector<unsigned int> indices;
    unsigned int index = 0;

    for (size_t i = 0; i < voxels.size(); ++i) {
        std::vector<unsigned int> cubeIndices = {
            0, 1, 2, 2, 3, 0, // Front face
            4, 5, 6, 6, 7, 4, // Back face
            0, 1, 5, 5, 4, 0, // Bottom face
            2, 3, 7, 7, 6, 2, // Top face
            0, 3, 7, 7, 4, 0, // Left face
            1, 2, 6, 6, 5, 1  // Right face
        };

        for (auto idx : cubeIndices) {
            indices.push_back(idx + index);
        }

        index += 8; // Each cube adds 8 vertices
    }

    return indices;
}
