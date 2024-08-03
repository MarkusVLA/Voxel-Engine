#include "chunk.h"
#include "chunk_manager.h"


Chunk::Chunk(int width, int height, int depth, glm::vec2 index, ChunkManager* manager)
    : width(width), height(height), depth(depth), index_(index), manager(manager), terrainGenerator(width, height, depth, index) {
    std::srand(static_cast<unsigned>(std::time(0)));
    terrainGenerator.generateTerrain(voxels);
}

Chunk::~Chunk() {
    for (Voxel* voxel : voxels) {
        delete voxel;
    }
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
                    if (voxel->isTranslucent()) {
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
    std::vector<float> voxelVertices = voxel->getVertexData(offset, faceFlags, 1.0f);
    vertices.insert(vertices.end(), voxelVertices.begin(), voxelVertices.end());

    // Get index data
    std::vector<unsigned int> voxelIndices = voxel->getIndexData(baseIndex, faceFlags);
    indices.insert(indices.end(), voxelIndices.begin(), voxelIndices.end());

    baseIndex += popcount(faceFlags) * 4;
}

uint8_t Chunk::getFaceFlags(glm::vec3 pos) const {
    uint8_t flags = 0;
    Voxel* voxel = getVoxel(pos);

    if (voxel->getIsXShaped()) {
        flags |= FACE_DIAGONAL_1 | FACE_DIAGONAL_2;
    }
    else if (voxel->getType() == WATER) {
        flags |= (pos.y == height - 1 || getVoxel({pos.x, pos.y + 1, pos.z}) == nullptr || getVoxel({pos.x, pos.y + 1, pos.z})->getType() != WATER) ? FACE_TOP : 0;
    } 
    else {
        // Check front face
        if (pos.z == depth - 1) {
            std::shared_ptr<Chunk> frontChunk = manager->getChunk(index_ + glm::vec2(0, 1));
            flags |= (frontChunk == nullptr || frontChunk->getVoxel({pos.x, pos.y, 0}) == nullptr || frontChunk->getVoxel({pos.x, pos.y, 0})->getType() == WATER || frontChunk->getVoxel({pos.x, pos.y, 0})->getIsXShaped()) ? FACE_FRONT : 0;
        } else {
            flags |= (getVoxel({pos.x, pos.y, pos.z + 1}) == nullptr || getVoxel({pos.x, pos.y, pos.z + 1})->getType() == WATER || getVoxel({pos.x, pos.y, pos.z + 1})->getIsXShaped()) ? FACE_FRONT : 0;
        }

        // Check back face
        if (pos.z == 0) {
            std::shared_ptr<Chunk> backChunk = manager->getChunk(index_ + glm::vec2(0, -1));
            flags |= (backChunk == nullptr || backChunk->getVoxel({pos.x, pos.y, depth - 1}) == nullptr || backChunk->getVoxel({pos.x, pos.y, depth - 1})->getType() == WATER || backChunk->getVoxel({pos.x, pos.y, depth - 1})->getIsXShaped()) ? FACE_BACK : 0;
        } else {
            flags |= (getVoxel({pos.x, pos.y, pos.z - 1}) == nullptr || getVoxel({pos.x, pos.y, pos.z - 1})->getType() == WATER || getVoxel({pos.x, pos.y, pos.z - 1})->getIsXShaped()) ? FACE_BACK : 0;
        }

        // Check left face
        if (pos.x == 0) {
            std::shared_ptr<Chunk> leftChunk = manager->getChunk(index_ + glm::vec2(-1, 0));
            flags |= (leftChunk == nullptr || leftChunk->getVoxel({width - 1, pos.y, pos.z}) == nullptr || leftChunk->getVoxel({width - 1, pos.y, pos.z})->getType() == WATER || leftChunk->getVoxel({width - 1, pos.y, pos.z})->getIsXShaped()) ? FACE_LEFT : 0;
        } else {
            flags |= (getVoxel({pos.x - 1, pos.y, pos.z}) == nullptr || getVoxel({pos.x - 1, pos.y, pos.z})->getType() == WATER || getVoxel({pos.x - 1, pos.y, pos.z})->getIsXShaped()) ? FACE_LEFT : 0;
        }

        // Check right face
        if (pos.x == width - 1) {
            std::shared_ptr<Chunk> rightChunk = manager->getChunk(index_ + glm::vec2(1, 0));
            flags |= (rightChunk == nullptr || rightChunk->getVoxel({0, pos.y, pos.z}) == nullptr || rightChunk->getVoxel({0, pos.y, pos.z})->getType() == WATER || rightChunk->getVoxel({0, pos.y, pos.z})->getIsXShaped()) ? FACE_RIGHT : 0;
        } else {
            flags |= (getVoxel({pos.x + 1, pos.y, pos.z}) == nullptr || getVoxel({pos.x + 1, pos.y, pos.z})->getType() == WATER || getVoxel({pos.x + 1, pos.y, pos.z})->getIsXShaped()) ? FACE_RIGHT : 0;
        }

        // Top and bottom faces remain the same as they don't cross chunk boundaries
        flags |= (pos.y == height - 1 || getVoxel({pos.x, pos.y + 1, pos.z}) == nullptr || getVoxel({pos.x, pos.y + 1, pos.z})->getType() == WATER || getVoxel({pos.x, pos.y + 1, pos.z})->getIsXShaped()) ? FACE_TOP : 0;
        flags |= (pos.y == 0 || getVoxel({pos.x, pos.y - 1, pos.z}) == nullptr || getVoxel({pos.x, pos.y - 1, pos.z})->getType() == WATER || getVoxel({pos.x, pos.y - 1, pos.z})->getIsXShaped()) ? FACE_BOTTOM : 0;
    }

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
