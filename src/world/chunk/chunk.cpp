#include "chunk.h"
#include "chunk_manager.h"
#include <iostream>
#include <ostream>


Chunk::Chunk(int width, int height, int depth, glm::vec2 index, ChunkManager* manager, unsigned int seed)
    : width(width), height(height), depth(depth), index_(index), manager(manager),
      terrainGenerator(width, height, depth, index, seed) {
    std::srand(static_cast<unsigned>(std::time(0)));
    terrainGenerator.generateTerrain(voxels, voxelsOutsideChunk);
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

    baseIndex += popcount(faceFlags) * 4; // 4 vertices per face.
}

uint8_t Chunk::getFaceFlags(glm::vec3 pos) const {
    uint8_t flags = 0;
    Voxel* voxel = getVoxel(pos);

    if (voxel->getIsXShaped()) {
        return FACE_DIAGONAL_1 | FACE_DIAGONAL_2;
    }

    if (voxel->getType() == WATER) {
        bool isTopWaterExposed = pos.y == height - 1 ||
                                 !getVoxel({pos.x, pos.y + 1, pos.z}) ||
                                 getVoxel({pos.x, pos.y + 1, pos.z})->getType() != WATER;
        return isTopWaterExposed ? FACE_TOP : 0;
    }

    flags |= checkFace(pos, {0, 0, 1}, FACE_FRONT);
    flags |= checkFace(pos, {0, 0, -1}, FACE_BACK);
    flags |= checkFace(pos, {-1, 0, 0}, FACE_LEFT);
    flags |= checkFace(pos, {1, 0, 0}, FACE_RIGHT);
    flags |= checkFace(pos, {0, 1, 0}, FACE_TOP);
    flags |= checkFace(pos, {0, -1, 0}, FACE_BOTTOM);

    return flags;
}

uint8_t Chunk::checkFace(glm::vec3 pos, glm::vec3 offset, uint8_t faceFlag) const {
    glm::vec3 neighborPos = pos + offset;

    if (isOutOfBounds(neighborPos)) {
        std::shared_ptr<Chunk> neighborChunk = getNeighborChunk(neighborPos);
        if (!neighborChunk) return faceFlag;

        glm::vec3 wrappedPos = wrapPosition(neighborPos);
        return shouldRenderFace(neighborChunk->getVoxel(wrappedPos)) ? faceFlag : 0;
    }

    return shouldRenderFace(getVoxel(neighborPos)) ? faceFlag : 0;
}

bool Chunk::isOutOfBounds(glm::vec3 pos) const {
    return pos.x < 0 || pos.x >= width ||
           pos.y < 0 || pos.y >= height ||
           pos.z < 0 || pos.z >= depth;
}

std::shared_ptr<Chunk> Chunk::getNeighborChunk(glm::vec3 pos) const {
    glm::vec2 offset(0);
    if (pos.x < 0) offset.x = -1;
    else if (pos.x >= width) offset.x = 1;
    else if (pos.z < 0) offset.y = -1;
    else if (pos.z >= depth) offset.y = 1;

    return manager->getChunk(index_ + offset);
}

glm::vec3 Chunk::wrapPosition(glm::vec3 pos) const {
    return glm::vec3(
        static_cast<float>(static_cast<int>(pos.x + width) % width),
        pos.y,
        static_cast<float>(static_cast<int>(pos.z + depth) % depth)
    );
}

void Chunk::updateMesh() {

    bool allNeighborsReady = true;
    for (const auto& offset : {glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1)}) {
        if (!manager->getChunk(index_ + offset)) {
            allNeighborsReady = false;
            break;
        }
    }

    if (allNeighborsReady) {
        cachedMesh = getMesh();
    }
}


bool Chunk::shouldRenderFace(const Voxel* voxel) const {
    return !voxel || voxel->getType() == WATER || voxel->getType() == LEAVES || voxel->getIsXShaped();
    // Transparent voxels should not cause faces to be hidden.
    // Optimise leaves to occlude each other later.
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


void Chunk::placeOutsideVoxels() {
    for (Voxel* voxel : voxelsOutsideChunk) {
        glm::vec2 chunkOffset(0);
        glm::vec3 localPos = voxel->getPosition();

        if (localPos.x < 0) chunkOffset.x = -1;
        else if (localPos.x >= width) chunkOffset.x = 1;
        if (localPos.z < 0) chunkOffset.y = -1;
        else if (localPos.z >= depth) chunkOffset.y = 1;

        std::shared_ptr<Chunk> neighborChunk = manager->getChunk(index_ + chunkOffset);

        if (neighborChunk) {
            glm::vec3 adjustedPos = localPos;
            adjustedPos.x = (static_cast<int>(adjustedPos.x) + width) %  width;
            adjustedPos.z = (static_cast<int>(adjustedPos.z) + depth) % depth;

            neighborChunk->setVoxel(adjustedPos, voxel->getType());
        }
    }
    voxelsOutsideChunk.clear();
}
