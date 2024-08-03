#include "renderer.h"
#include "texture_loader.h"
#include <iostream>

#include "renderer.h"
#include "texture_loader.h"
#include <iostream>

Renderer::Renderer() 
    : shouldExit(false), textureLoaded(false), objectShader(nullptr), skyboxShader(nullptr), waterShader(nullptr) {
    initOpenGL();
    camera = nullptr;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 500.0f);
    lightDir = glm::normalize(glm::vec3(1.0f, 0.5f, 0.3f));
}

Renderer::~Renderer() {
    for (auto& [chunkPos, mesh] : chunkMeshes) {
        glDeleteVertexArrays(1, &mesh.solidVAO);
        glDeleteBuffers(1, &mesh.solidVBO);
        glDeleteBuffers(1, &mesh.solidEBO);
        glDeleteVertexArrays(1, &mesh.waterVAO);
        glDeleteBuffers(1, &mesh.waterVBO);
        glDeleteBuffers(1, &mesh.waterEBO);
    }
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    delete objectShader;
    delete skyboxShader;
    delete waterShader;

}

void Renderer::initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glfwSwapInterval(0);
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error after initialization: " << err << std::endl;
    }
}

void Renderer::initShaders() {
    try {
        objectShader = new Shader("../src/shaders/triangle.vert", "../src/shaders/triangle.frag");
        skyboxShader = new Shader("../src/shaders/skybox.vert", "../src/shaders/skybox.frag");
        waterShader = new Shader("../src/shaders/water.vert", "../src/shaders/water.frag");
    } catch (const std::exception& e) {
        std::cerr << "Shader compilation failed: " << e.what() << std::endl;
        throw;
    }
}

void Renderer::addChunk(const glm::ivec2& chunkPos, const std::vector<float>& solidVertices, const std::vector<unsigned int>& solidIndices,
                        const std::vector<float>& waterVertices, const std::vector<unsigned int>& waterIndices) {
    std::unique_lock<std::mutex> lock(queueMutex);
    chunkUpdateQueue.push(ChunkUpdate{ChunkUpdateType::Add, chunkPos, solidVertices, solidIndices, waterVertices, waterIndices});
}

void Renderer::removeChunk(const glm::ivec2& chunkPos) {
    std::unique_lock<std::mutex> lock(queueMutex);
    chunkUpdateQueue.push(ChunkUpdate{ChunkUpdateType::Remove, chunkPos, {}, {}});
}

void Renderer::updateChunk(const glm::ivec2& chunkPos, const std::vector<float>& solidVertices, const std::vector<unsigned int>& solidIndices,
                           const std::vector<float>& waterVertices, const std::vector<unsigned int>& waterIndices) {
    std::unique_lock<std::mutex> lock(queueMutex);
    chunkUpdateQueue.push(ChunkUpdate{ChunkUpdateType::Update, chunkPos, solidVertices, solidIndices, waterVertices, waterIndices});
}

void Renderer::processChunkUpdates() {
    std::queue<ChunkUpdate> updates;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        std::swap(updates, chunkUpdateQueue);
    }

    while (!updates.empty()) {
        const auto& update = updates.front();
        switch (update.type) {
            case ChunkUpdateType::Add:
                addChunkImpl(update.chunkPos, update.solidVertices, update.solidIndices, update.waterVertices, update.waterIndices);
                break;
            case ChunkUpdateType::Update:
                updateChunkImpl(update.chunkPos, update.solidVertices, update.solidIndices, update.waterVertices, update.waterIndices);
                break;
            case ChunkUpdateType::Remove:
                removeChunkImpl(update.chunkPos);
                break;
        }
        updates.pop();
    }
}

void Renderer::addChunkImpl(const glm::ivec2& chunkPos, const std::vector<float>& solidVertices, const std::vector<unsigned int>& solidIndices,
                            const std::vector<float>& waterVertices, const std::vector<unsigned int>& waterIndices) {
    ChunkMesh mesh;

    // Solid mesh
    glGenVertexArrays(1, &mesh.solidVAO);
    glGenBuffers(1, &mesh.solidVBO);
    glGenBuffers(1, &mesh.solidEBO);

    glBindVertexArray(mesh.solidVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.solidVBO);
    glBufferData(GL_ARRAY_BUFFER, solidVertices.size() * sizeof(float), solidVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.solidEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, solidIndices.size() * sizeof(unsigned int), solidIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    mesh.solidIndexCount = solidIndices.size();

    // Water mesh
    glGenVertexArrays(1, &mesh.waterVAO);
    glGenBuffers(1, &mesh.waterVBO);
    glGenBuffers(1, &mesh.waterEBO);

    glBindVertexArray(mesh.waterVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.waterVBO);
    glBufferData(GL_ARRAY_BUFFER, waterVertices.size() * sizeof(float), waterVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.waterEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterIndices.size() * sizeof(unsigned int), waterIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    mesh.waterIndexCount = waterIndices.size();
    
    std::unique_lock<std::mutex> lock(chunkMutex);
    chunkMeshes[chunkPos] = mesh;
}

void Renderer::updateChunkImpl(const glm::ivec2& chunkPos, const std::vector<float>& solidVertices, const std::vector<unsigned int>& solidIndices,
                               const std::vector<float>& waterVertices, const std::vector<unsigned int>& waterIndices) {
    std::unique_lock<std::mutex> lock(chunkMutex);
    auto it = chunkMeshes.find(chunkPos);
    if (it != chunkMeshes.end()) {
        // Update solid mesh
        glBindVertexArray(it->second.solidVAO);
        glBindBuffer(GL_ARRAY_BUFFER, it->second.solidVBO);
        glBufferData(GL_ARRAY_BUFFER, solidVertices.size() * sizeof(float), solidVertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second.solidEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, solidIndices.size() * sizeof(unsigned int), solidIndices.data(), GL_STATIC_DRAW);
        it->second.solidIndexCount = solidIndices.size();

        // Update water mesh
        glBindVertexArray(it->second.waterVAO);
        glBindBuffer(GL_ARRAY_BUFFER, it->second.waterVBO);
        glBufferData(GL_ARRAY_BUFFER, waterVertices.size() * sizeof(float), waterVertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second.waterEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterIndices.size() * sizeof(unsigned int), waterIndices.data(), GL_STATIC_DRAW);
        it->second.waterIndexCount = waterIndices.size();
    } else {
        lock.unlock();
        addChunkImpl(chunkPos, solidVertices, solidIndices, waterVertices, waterIndices);
    }
}

void Renderer::removeChunkImpl(const glm::ivec2& chunkPos) {
    std::unique_lock<std::mutex> lock(chunkMutex);
    auto it = chunkMeshes.find(chunkPos);
    if (it != chunkMeshes.end()) {
        glDeleteVertexArrays(1, &it->second.solidVAO);
        glDeleteBuffers(1, &it->second.solidVBO);
        glDeleteBuffers(1, &it->second.solidEBO);

        glDeleteVertexArrays(1, &it->second.waterVAO);
        glDeleteBuffers(1, &it->second.waterVBO);
        glDeleteBuffers(1, &it->second.waterEBO);

        chunkMeshes.erase(it);
    }
}

void Renderer::setSkyboxData(const std::vector<float>& vertices) {
    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::loadTexture(const std::string& path) {
    std::unique_lock<std::mutex> lock(textureMutex);
    textureID = TextureLoader::loadTextureAtlas(path);
    textureLoaded = true;
}

void Renderer::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);
    if (skyboxShader && camera) {
        skyboxShader->use();
        glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix()));
        skyboxShader->setMat4("view", view);
        skyboxShader->setMat4("projection", projection);
        skyboxShader->setFloat("time", static_cast<float>(glfwGetTime()));
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    if (!textureLoaded || !camera) {
        return;
    }
    glm::mat4 view = camera->getViewMatrix();
    std::unique_lock<std::mutex> lock(chunkMutex);

    if (objectShader) {
        glDisable(GL_BLEND);  
        objectShader->use();
        setupShaderUniforms(objectShader, view, projection);

        for (const auto& [chunkPos, mesh] : chunkMeshes) {
            renderChunk(objectShader, mesh.solidVAO, mesh.solidIndexCount, chunkPos);
        }
    }

    // Collect and sort transparent chunks
    struct TransparentChunk {
        glm::ivec2 chunkPos;
        ChunkMesh mesh;
        float distance;
    };
    
    std::vector<TransparentChunk> transparentChunks;
    for (const auto& [chunkPos, mesh] : chunkMeshes) {
        if (mesh.waterIndexCount > 0) { 
            glm::vec3 chunkCenter = glm::vec3(chunkPos.x * 16 + 8, 0, chunkPos.y * 16 + 8);
            float distance = glm::distance(camera->getPosition(), chunkCenter);
            transparentChunks.push_back({chunkPos, mesh, distance});
        }
    }

    std::sort(transparentChunks.begin(), transparentChunks.end(),
        [](const TransparentChunk& a, const TransparentChunk& b) {
            return a.distance > b.distance; 
        });


    if (waterShader) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (const auto& chunk : transparentChunks) {
            renderChunk(waterShader, chunk.mesh.waterVAO, chunk.mesh.waterIndexCount, chunk.chunkPos);
        }

        glDepthMask(GL_TRUE); 
        glDisable(GL_BLEND);
    }

    glBindVertexArray(0);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}


void Renderer::setupShaderUniforms(Shader* shader, const glm::mat4& view, glm::mat4& projection) {
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader->setVec3("lightDir", lightDir);
    shader->setVec3("cameraPos", camera->getPosition());
    shader->setVec3("lightColor", glm::vec3(1.0f, 0.8f, 0.6f));
    shader->setFloat("ambientStrength", 0.2f);
    shader->setVec3("fogColor", glm::vec3(0.7f, 0.8f, 0.9f));
    shader->setFloat("fogDensity", 0.003f);
    shader->setFloat("time", static_cast<float>(glfwGetTime())); 
}



void Renderer::renderChunk(Shader* shader, GLuint vao, int indexCount, const glm::ivec2& chunkPos) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(chunkPos.x * 16, 0, chunkPos.y * 16));
    shader->setMat4("model", model);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}


void Renderer::setViewport(int width, int height) {
    glViewport(0, 0, width, height);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 500.0f);
}

void Renderer::setCamera(Camera* cam) {
    camera = cam;
}

void Renderer::setLightDir(glm::vec3 dir) {
    lightDir = dir;
}
