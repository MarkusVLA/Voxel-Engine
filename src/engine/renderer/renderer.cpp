#include "renderer.h"
#include "texture_loader.h"
#include <iostream>

Renderer::Renderer() {
    initOpenGL();
    camera = nullptr;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
    indicesSize = 0;
    objectShader = new Shader("../src/shaders/triangle.vert", "../src/shaders/triangle.frag");
    skyboxShader = new Shader("../src/shaders/skybox.vert", "../src/shaders/skybox.frag");
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    delete objectShader;
    delete skyboxShader;
}

void Renderer::initOpenGL() {
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LESS);   
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
}

void Renderer::setMeshData(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    indicesSize = indices.size(); 
}


void Renderer::setSkyboxData(const std::vector<float>& vertices) {
    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::loadTexture(const std::string& path) {
    textureID = TextureLoader::loadTexture(path);
}

void Renderer::draw() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();
    if (camera) {
        glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix()));
        skyboxShader->setMat4("view", view);
        skyboxShader->setMat4("projection", projection); 
    }
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    objectShader->use();
    glBindVertexArray(VAO);
    if (camera) {
        glm::mat4 view = camera->getViewMatrix();
        objectShader->setMat4("view", view);
        objectShader->setMat4("projection", projection); 
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
}


void Renderer::setViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::setCamera(Camera* cam) {
    camera = cam;
}
