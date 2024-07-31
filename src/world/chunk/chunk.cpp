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
    voxels.resize(width * height * depth, nullptr);
    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            int maxY = generateHeight(x, z);
            for (int y = 0; y < maxY; ++y) {
                glm::vec3 voxelPosition(x, y, z);
                unsigned int voxelIndex = coordsToIndex(voxelPosition);
                if (y < maxY / 2) {
                    voxels[voxelIndex] = new Voxel(voxelPosition, STONE);
                } else {
                    voxels[voxelIndex] = new Voxel(voxelPosition, GRASS);
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
    // ofsett based on chunk location:
    glm::vec3 offset(index_.x * width, 0, index_.y * depth);
    for (const Voxel* voxel : voxels) {
        if (voxel != nullptr) {
            std::vector<float> voxelVertices = voxel->getVertexData(offset);
            vertices.insert(vertices.end(), voxelVertices.begin(), voxelVertices.end());
        }
    }
    return vertices;
}

std::vector<unsigned int> Chunk::getIndexData() const {
    std::vector<unsigned int> indices;
    unsigned int baseIndex = 0;
    for (const Voxel* voxel : voxels) {
        if (voxel != nullptr) {
            std::vector<unsigned int> voxelIndices = voxel->getIndexData(baseIndex);
            indices.insert(indices.end(), voxelIndices.begin(), voxelIndices.end());
            baseIndex += 24; // Number of vertices per voxel
        }
    }
    return indices;
}


Voxel* Chunk::getVoxel(const glm::vec3& pos) const {
    unsigned int index = coordsToIndex(pos);
    if (index < voxels.size()) {
        return voxels[index];
    }
    return nullptr;
}

void Chunk::setVoxel(const glm::vec3& pos, VoxelType type) {
    unsigned int index = coordsToIndex(pos);
    if (index < voxels.size()) {
        delete voxels[index];
        voxels[index] = new Voxel(pos, type);
    }
}

std::vector<float> Chunk::getMesh() const {
    std::vector<float> mesh = getVertexData();
    std::vector<unsigned int> indices = getIndexData();
    mesh.insert(mesh.end(), indices.begin(), indices.end());
    return mesh;
}

unsigned int Chunk::coordsToIndex(glm::vec3 coords) const {
    return static_cast<unsigned int>(
        coords.x + 
        coords.y * width + 
        coords.z * width * height
    );
}

glm::vec3 Chunk::indexToCoords(int index) const {
    return glm::vec3(
        index % width,
        (index / width) % height,
        index / (width * height)
    );
}

glm::vec2 Chunk::getIndex() const {
    return index_;
}

bool Chunk::operator==(const Chunk& other) const {
    return index_ == other.index_;
}
