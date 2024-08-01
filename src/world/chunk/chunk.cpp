#include "chunk.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

Chunk::Chunk(int width, int height, int depth, glm::vec2 index)
    : width(width), height(height), depth(depth), index_(index), perlinNoise() {
    std::srand(static_cast<unsigned>(std::time(0)));
    generateTerrain();
}


Chunk::~Chunk(){
    for (Voxel * voxel: voxels){
        delete voxel;
    }
}


void Chunk::generateTerrain() {
    voxels.resize(width * height * depth, nullptr);
    
    // First pass: Generate basic terrain and large cave structures
    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            int maxY = generateHeight(x, z);
            for (int y = 0; y < height; ++y) {
                glm::vec3 voxelPosition(x, y, z);
                unsigned int voxelIndex = coordsToIndex(voxelPosition);
                
                if (y < maxY) {
                    // Determine the block type based on height
                    VoxelType blockType;
                    if (y == maxY - 1) {
                        blockType = GRASS;
                    } else if (y > maxY - 4) {
                        blockType = DIRT;
                    } else {
                        blockType = STONE;
                    }

                    // Generate caves
                    float caveNoise1 = perlinNoise.noise((x + index_.x * width) * 0.03f, 
                                                         y * 0.03f, 
                                                         (z + index_.y * depth) * 0.03f);
                    float caveNoise2 = perlinNoise.noise((x + index_.x * width) * 0.05f, 
                                                         y * 0.05f, 
                                                         (z + index_.y * depth) * 0.05f);
                    float caveDensity = (caveNoise1 + 0.5 * caveNoise2) / 1.5;

                    if (caveDensity > 0.55f && y < maxY - 5) {
                        // This is a cave, don't place a block
                        continue;
                    }

                    // Place the block
                    voxels[voxelIndex] = new Voxel(voxelPosition, blockType);

                    // Generate ore veins
                    if (blockType == STONE) {
                        float oreNoise = perlinNoise.noise((x + index_.x * width) * 0.2f, 
                                                           y * 0.2f, 
                                                           (z + index_.y * depth) * 0.2f);
                        if (oreNoise > 0.8f) {
                            voxels[voxelIndex] = new Voxel(voxelPosition, IRONORE);
                        }
                    }
                }
            }
        }
    }
}


int Chunk::generateHeight(int x, int z) const {
    float scale1 = 0.01f;
    float scale2 = 0.05f;
    float globalX = (x + index_.x * width);
    float globalZ = (z + index_.y * depth);

    float baseHeight = 64.0f;
    float heightVariation = 32.0f;
    float mountainousness = 16.0f;

    float noise1 = perlinNoise.noise(globalX * scale1, globalZ * scale1);
    float noise2 = perlinNoise.noise(globalX * scale2, globalZ * scale2);

    return static_cast<int>(
        (noise1 * heightVariation + baseHeight) +
        (std::pow(std::max(0.0f, noise2), 2) * mountainousness)
    );
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
                    uint8_t faceFlags = getFaceFlags(glm::vec3(x,y,z));
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
                    uint8_t faceFlags = getFaceFlags(glm::vec3(x,y,z));
                    std::vector<unsigned int> voxelIndices = voxel->getIndexData(baseIndex, faceFlags);
                    indices.insert(indices.end(), voxelIndices.begin(), voxelIndices.end());
                    baseIndex += popcount(faceFlags) * 4; // 4 vertices per face
                }
            }
        }
    }
    return indices;
}

uint8_t Chunk::getFaceFlags(glm::vec3 pos) const {
    uint8_t flags = 0B00000000;
    flags |= (pos.z == depth - 1 || getVoxel({pos.x, pos.y, pos.z + 1}) == nullptr) ? FACE_FRONT : 0;
    flags |= (pos.z == 0 || getVoxel({pos.x, pos.y, pos.z - 1}) == nullptr) ? FACE_BACK : 0;
    flags |= (pos.x == 0 || getVoxel({pos.x - 1, pos.y, pos.z}) == nullptr) ? FACE_LEFT : 0;
    flags |= (pos.x == width - 1 || getVoxel({pos.x + 1, pos.y, pos.z}) == nullptr) ? FACE_RIGHT : 0;
    flags |= (pos.y == height - 1 || getVoxel({pos.x, pos.y + 1, pos.z}) == nullptr) ? FACE_TOP : 0;
    flags |= (pos.y == 0 || getVoxel({pos.x, pos.y - 1, pos.z}) == nullptr) ? FACE_BOTTOM : 0;
    return flags;
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
