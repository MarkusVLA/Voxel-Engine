#include "chunk.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

Chunk::Chunk(int width, int height, int depth, glm::vec2 index)
    : width(width), height(height), depth(depth), index_(index), perlinNoise() {
    std::srand(static_cast<unsigned>(std::time(0)));
    generateTerrain();
}

void Chunk::generateTerrain() {

    // Reserve space for all chunk voxels
    voxels.reserve(width * height * depth);
    

    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            int y = generateHeight(x, z);
            for (int h = 0; h < y; ++h) {
                glm::vec3 voxelPosition(x, h, z);
                if (h < y / 2) {  
                    voxels.emplace_back(voxelPosition, STONE);
                } else {
                    voxels.emplace_back(voxelPosition, GRASS);
                }
            }
        }
    }
}



int Chunk::generateHeight(int x, int z) const {
    float scale = 0.02f; 
    float globalX = (x + index_.x * width) * scale;
    float globalZ = (z + index_.y * depth) * scale;
    return static_cast<int>(perlinNoise.noise(globalX, globalZ) * 15 + 10);
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
        baseIndex += 24;
    }
    return indices;
}

std::vector<float> Chunk::getMesh() const {
    // Assuming this function should return a combination of vertex and index data
    std::vector<float> mesh = getVertexData();
    std::vector<unsigned int> indices = getIndexData();
    mesh.insert(mesh.end(), indices.begin(), indices.end());
    return mesh;
}


glm::vec3 Chunk::indexToCoords(int index) const {
    return glm::vec3(
        static_cast<float>(index % 16),
        static_cast<float>(index / (16 * 16)),
        static_cast<float>((index / 16) % 16)
    );
}

unsigned int Chunk::coordsToIndex(const glm::vec3 coords) const {
    return static_cast<int>(
        coords.x + 
        coords.y * 16 * 16 + 
        coords.z * 16
    );
}

glm::vec2 Chunk::getIndex() const {
    return index_;
}

bool Chunk::operator==(const Chunk& other) const {
    return index_ == other.index_;
}
