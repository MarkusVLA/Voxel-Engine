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
    glm::vec3 offset(index_.x * width, 0, index_.y * depth);
    
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                glm::vec3 pos(x, y, z);
                Voxel* voxel = getVoxel(pos);
                if (voxel != nullptr) {
                    uint8_t faceFlags = 0;
                    faceFlags |= (z == depth - 1 || getVoxel({x, y, z + 1}) == nullptr) ? FACE_FRONT : 0;
                    faceFlags |= (z == 0 || getVoxel({x, y, z - 1}) == nullptr) ? FACE_BACK : 0;
                    faceFlags |= (x == 0 || getVoxel({x - 1, y, z}) == nullptr) ? FACE_LEFT : 0;
                    faceFlags |= (x == width - 1 || getVoxel({x + 1, y, z}) == nullptr) ? FACE_RIGHT : 0;
                    faceFlags |= (y == height - 1 || getVoxel({x, y + 1, z}) == nullptr) ? FACE_TOP : 0;
                    faceFlags |= (y == 0 || getVoxel({x, y - 1, z}) == nullptr) ? FACE_BOTTOM : 0;

                    std::vector<float> voxelVertices = voxel->getVertexData(offset, faceFlags);
                    vertices.insert(vertices.end(), voxelVertices.begin(), voxelVertices.end());
                }
            }
        }
    }
    return vertices;
}

std::vector<unsigned int> Chunk::getIndexData() const {
    std::vector<unsigned int> indices;
    unsigned int baseIndex = 0;
    
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                glm::vec3 pos(x, y, z);
                Voxel* voxel = getVoxel(pos);
                if (voxel != nullptr) {
                    // Check which faces need to be drawn
                    uint8_t faceFlags = 0;
                    faceFlags |= (z == depth - 1 || getVoxel({x, y, z + 1}) == nullptr) ? FACE_FRONT : 0;
                    faceFlags |= (z == 0 || getVoxel({x, y, z - 1}) == nullptr) ? FACE_BACK : 0;
                    faceFlags |= (x == 0 || getVoxel({x - 1, y, z}) == nullptr) ? FACE_LEFT : 0;
                    faceFlags |= (x == width - 1 || getVoxel({x + 1, y, z}) == nullptr) ? FACE_RIGHT : 0;
                    faceFlags |= (y == height - 1 || getVoxel({x, y + 1, z}) == nullptr) ? FACE_TOP : 0;
                    faceFlags |= (y == 0 || getVoxel({x, y - 1, z}) == nullptr) ? FACE_BOTTOM : 0;

                    std::vector<unsigned int> voxelIndices = voxel->getIndexData(baseIndex, faceFlags);
                    indices.insert(indices.end(), voxelIndices.begin(), voxelIndices.end());
                    baseIndex += __builtin_popcount(faceFlags) * 4; // 4 vertices per face
                }
            }
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
