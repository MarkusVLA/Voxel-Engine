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

    const int waterLevel = 64; // Adjust this value to set the water level

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
                    float caveDensity = (caveNoise1 + 0.5f * caveNoise2) / 1.5f;

                    if (caveDensity > 0.55f && y < maxY - 5) {
                        // If it's a cave and below water level, fill with water
                        if (y < waterLevel) {
                            voxels[voxelIndex] = new Voxel(voxelPosition, WATER);
                        }
                        continue;
                    }

                    voxels[voxelIndex] = new Voxel(voxelPosition, blockType);

                    if (blockType == STONE) {
                        float oreNoise = perlinNoise.noise((x + index_.x * width) * 0.2f, 
                                                           y * 0.2f, 
                                                           (z + index_.y * depth) * 0.2f);
                        if (oreNoise > 0.8f) {
                            voxels[voxelIndex] = new Voxel(voxelPosition, IRONORE);
                        }
                    }
                } else if (y < waterLevel) {
                    // Fill empty spaces below water level with water
                    voxels[voxelIndex] = new Voxel(voxelPosition, WATER);
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


std::tuple<std::vector<float>, std::vector<unsigned int>, std::vector<float>, std::vector<unsigned int>> Chunk::getMesh() const {
    std::vector<float> solidVertices;
    std::vector<unsigned int> solidIndices;
    std::vector<float> waterVertices;
    std::vector<unsigned int> waterIndices;
    glm::vec3 offset(index_.x * width, 0, index_.y * depth);
    unsigned int solidBaseIndex = 0;
    unsigned int waterBaseIndex = 0;
    
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                glm::vec3 pos(x, y, z);
                Voxel* voxel = getVoxel(pos);
                if (voxel != nullptr) {
                    uint8_t faceFlags = getFaceFlags(pos);
                    
                    if (voxel->getType() == WATER) {
                        addVoxelMesh(voxel, offset, faceFlags, waterVertices, waterIndices, waterBaseIndex);
                    } else {
                        addVoxelMesh(voxel, offset, faceFlags, solidVertices, solidIndices, solidBaseIndex);
                    }
                }
            }
        }
    }
    
    return std::make_tuple(solidVertices, solidIndices, waterVertices, waterIndices);
}

void Chunk::addVoxelMesh(Voxel* voxel, const glm::vec3& offset, uint8_t faceFlags, 
                         std::vector<float>& vertices, std::vector<unsigned int>& indices, 
                         unsigned int& baseIndex) const {
    // Get vertex data
    std::vector<float> voxelVertices = voxel->getVertexData(offset, faceFlags);
    vertices.insert(vertices.end(), voxelVertices.begin(), voxelVertices.end());
    
    // Get index data
    std::vector<unsigned int> voxelIndices = voxel->getIndexData(baseIndex, faceFlags);
    indices.insert(indices.end(), voxelIndices.begin(), voxelIndices.end());
    
    baseIndex += popcount(faceFlags) * 4; // 4 vertices per face
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
