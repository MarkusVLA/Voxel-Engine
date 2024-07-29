#include "chunk.h"
#include <cstdlib>
#include <ctime>

Chunk::Chunk(int width, int height, int depth, glm::vec2 index) 
    : width(width), height(height), depth(depth), index_(index) {
    std::srand(static_cast<unsigned>(std::time(0)));
    generateTerrain();
}

void Chunk::generateTerrain() {
    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            int y = generateHeight(x, z);
            for (int h = 0; h < y; ++h) {
                glm::vec3 voxelPosition(x, h, z); // local position within the chunk
                voxels.emplace_back(voxelPosition, 1); // Type 1 for terrain blocks
            }
        }
    }
}

int Chunk::generateHeight(int x, int z) const {
    // Use the global coordinates to generate the height
    float scale = 0.1f;
    float globalX = (x + index_.x * width);
    float globalZ = (z + index_.y * depth);
    return static_cast<int>((std::sin(globalX * scale) + std::cos(globalZ * scale)) * 5 + 10);
}

std::vector<float> Chunk::getVertexData() const {
    std::vector<float> vertices;
    glm::vec3 offset(index_.x * width, 0, index_.y * depth);

    for (const auto& voxel : voxels) {
        std::vector<float> voxelVertices = voxel.getVertexData(offset);
        vertices.insert(vertices.end(), voxelVertices.begin(), voxelVertices.end());
    }

    return vertices;
}

std::vector<unsigned int> Chunk::getIndexData() const {
    std::vector<unsigned int> indices;
    unsigned int baseIndex = 0;

    for (const auto& voxel : voxels) {
        std::vector<unsigned int> voxelIndices = voxel.getIndexData(baseIndex);
        indices.insert(indices.end(), voxelIndices.begin(), voxelIndices.end());
        baseIndex += 24; // Each voxel has 24 vertices
    }

    return indices;
}

glm::vec2 Chunk::getIndex() const {
    return index_;
}

bool Chunk::operator==(const Chunk& other) const {
    return width == other.width && height == other.height && depth == other.depth && index_ == other.index_;
}
