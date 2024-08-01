#include "renderer.h"
#include "texture_loader.h"
#include <iostream>

Renderer::Renderer() 
    : shouldExit(false), textureLoaded(false), objectShader(nullptr), skyboxShader(nullptr) {
    initOpenGL();
    camera = nullptr;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 500.0f);
}

Renderer::~Renderer() {
    for (auto& [chunkPos, mesh] : chunkMeshes) {
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBO);
        glDeleteBuffers(1, &mesh.EBO);
    }
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    delete objectShader;
    delete skyboxShader;
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
    } catch (const std::exception& e) {
        std::cerr << "Shader compilation failed: " << e.what() << std::endl;
        throw;
    }
}

void Renderer::addChunk(const glm::ivec2& chunkPos, const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    std::unique_lock<std::mutex> lock(queueMutex);
    chunkUpdateQueue.push(ChunkUpdate{ChunkUpdateType::Add, chunkPos, vertices, indices});
}

void Renderer::updateChunk(const glm::ivec2& chunkPos, const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    std::unique_lock<std::mutex> lock(queueMutex);
    chunkUpdateQueue.push(ChunkUpdate{ChunkUpdateType::Update, chunkPos, vertices, indices});
}

void Renderer::removeChunk(const glm::ivec2& chunkPos) {
    std::unique_lock<std::mutex> lock(queueMutex);
    chunkUpdateQueue.push(ChunkUpdate{ChunkUpdateType::Remove, chunkPos, {}, {}});
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
                addChunkImpl(update.chunkPos, update.vertices, update.indices);
                break;
            case ChunkUpdateType::Update:
                updateChunkImpl(update.chunkPos, update.vertices, update.indices);
                break;
            case ChunkUpdateType::Remove:
                removeChunkImpl(update.chunkPos);
                break;
        }
        updates.pop();
    }
}

void Renderer::addChunkImpl(const glm::ivec2& chunkPos, const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    ChunkMesh mesh;
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    mesh.indexCount = indices.size();
    
    std::unique_lock<std::mutex> lock(chunkMutex);
    chunkMeshes[chunkPos] = mesh;
}

void Renderer::updateChunkImpl(const glm::ivec2& chunkPos, const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    std::unique_lock<std::mutex> lock(chunkMutex);
    auto it = chunkMeshes.find(chunkPos);
    if (it != chunkMeshes.end()) {
        glBindVertexArray(it->second.VAO);

        glBindBuffer(GL_ARRAY_BUFFER, it->second.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        it->second.indexCount = indices.size();
    } else {
        lock.unlock();
        addChunkImpl(chunkPos, vertices, indices);
    }
}

void Renderer::removeChunkImpl(const glm::ivec2& chunkPos) {
    std::unique_lock<std::mutex> lock(chunkMutex);
    auto it = chunkMeshes.find(chunkPos);
    if (it != chunkMeshes.end()) {
        glDeleteVertexArrays(1, &it->second.VAO);
        glDeleteBuffers(1, &it->second.VBO);
        glDeleteBuffers(1, &it->second.EBO);
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

    // Draw skybox
    glDepthFunc(GL_LEQUAL);
    if (skyboxShader) {
        skyboxShader->use();
        if (camera) {
            glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix()));
            skyboxShader->setMat4("view", view);
            skyboxShader->setMat4("projection", projection);
        }
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    glDepthFunc(GL_LESS);

    // Draw chunks
    if (objectShader && textureLoaded) {
        objectShader->use();
        if (camera) { 
            glm::mat4 view = camera->getViewMatrix();
            objectShader->setMat4("view", view);
            objectShader->setMat4("projection", projection);
        }
        glBindTexture(GL_TEXTURE_2D, textureID);

        objectShader->setVec3("fogColor", glm::vec3(0.7f, 0.7f, 0.7f));
        objectShader->setFloat("fogDensity", 0.001f); 

        std::unique_lock<std::mutex> lock(chunkMutex);
        for (const auto& [chunkPos, mesh] : chunkMeshes) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(chunkPos.x * 16, 0, chunkPos.y * 16));
            objectShader->setMat4("model", model);

            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        }
    }
}

void Renderer::setViewport(int width, int height) {
    glViewport(0, 0, width, height);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 500.0f);
}

void Renderer::setCamera(Camera* cam) {
    camera = cam;
}