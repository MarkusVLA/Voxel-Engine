#include "chunk_manager.h"
#include <iostream>
#include <cmath> 

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int viewDistance)
    : chunkWidth(chunkWidth), chunkHeight(chunkHeight), chunkDepth(chunkDepth), viewDistance(viewDistance), chunksChanged(false), lastCameraChunk(0, 0), running(true) {
    chunkLoaderThread = std::thread(&ChunkManager::chunkLoader, this);
}

ChunkManager::~ChunkManager() {
    running = false;
    chunkCondition.notify_all();
    if (chunkLoaderThread.joinable()) {
        chunkLoaderThread.join();
    }
}

void ChunkManager::update(const glm::vec3& cameraPosition) {
    glm::ivec2 cameraChunk = glm::ivec2(static_cast<int>(std::floor(cameraPosition.x / chunkWidth)), static_cast<int>(std::floor(cameraPosition.z / chunkDepth)));

    if (cameraChunk == lastCameraChunk) {
        return;
    }

    lastCameraChunk = cameraChunk;
    chunksChanged = true;
    std::cout << "Camera chunk: " << cameraChunk.x << ", " << cameraChunk.y << std::endl;
    {
        std::lock_guard<std::mutex> lock(chunkMutex);
        chunkCondition.notify_all();
    }
}

bool ChunkManager::hasChunksChanged() const {
    return chunksChanged;
}

void ChunkManager::loadChunk(const glm::ivec2& index) {
    {
        std::lock_guard<std::mutex> lock(chunkMutex);
        chunks.emplace(index, Chunk(chunkWidth, chunkHeight, chunkDepth, glm::vec2(index)));
        chunksChanged = true;
    }
}

void ChunkManager::unloadChunk(const glm::ivec2& index) {
    {
        std::lock_guard<std::mutex> lock(chunkMutex);
        chunks.erase(index);
        chunksChanged = true;
    }
}

std::vector<float> ChunkManager::getVertexData() {
    std::vector<float> vertices;
    std::lock_guard<std::mutex> lock(chunkMutex);
    for (const auto& pair : chunks) {
        const auto& chunk = pair.second;
        std::vector<float> chunkVertices = chunk.getVertexData();
        vertices.insert(vertices.end(), chunkVertices.begin(), chunkVertices.end());
    }
    return vertices;
}

std::vector<unsigned int> ChunkManager::getIndexData() {
    std::vector<unsigned int> indices;
    unsigned int baseIndex = 0;
    std::lock_guard<std::mutex> lock(chunkMutex);
    for (const auto& pair : chunks) {
        const auto& chunk = pair.second;
        std::vector<unsigned int> chunkIndices = chunk.getIndexData();
        for (unsigned int index : chunkIndices) {
            indices.push_back(index + baseIndex);
        }
        baseIndex += chunk.getVertexData().size() / 8; // Assuming 8 floats per vertex
    }
    return indices;
}

void ChunkManager::chunkLoader() {
    while (running) {
        std::unique_lock<std::mutex> lock(chunkMutex);
        chunkCondition.wait(lock, [this] { return !running || chunksChanged; });

        if (!running) {
            break;
        }

        glm::ivec2 cameraChunk = lastCameraChunk;
        loadChunksAround(cameraChunk);
        chunksChanged = false;
    }
}

void ChunkManager::loadChunksAround(const glm::ivec2& cameraChunk) {
    std::unordered_map<glm::ivec2, Chunk> newChunks;

    for (int x = -viewDistance; x <= viewDistance; ++x) {
        for (int z = -viewDistance; z <= viewDistance; ++z) {
            glm::ivec2 chunkIndex = cameraChunk + glm::ivec2(x, z);
            auto it = chunks.find(chunkIndex);
            if (it == chunks.end()) {
                std::cout << "Loading new chunk at: " << chunkIndex.x << ", " << chunkIndex.y << std::endl;
                newChunks.emplace(chunkIndex, Chunk(chunkWidth, chunkHeight, chunkDepth, glm::vec2(chunkIndex)));
            } else {
                newChunks.emplace(chunkIndex, std::move(it->second));
            }
        }
    }

    chunks = std::move(newChunks);
}
