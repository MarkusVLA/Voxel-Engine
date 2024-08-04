#pragma once

#include "../../engine/camera/camera.h"
#include "../chunk/chunk_manager.h"
#include <glm/glm.hpp>

enum PlayerMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Player {
public:

    Player(Camera* camera, ChunkManager* chunkManager);
    ~Player();

    void update(float deltaTime);
    void move(PlayerMovement direction, float deltaTime);
    void applyGravity(float deltaTime);
    bool isOnGround() const;
    void setPosition(const glm::vec3& newPosition);
    glm::vec3 getPosition() const;
    Camera* getCamera();

private:
    Camera* camera;
    ChunkManager* chunkManager;
    glm::vec3 velocity;
    glm::vec3 size; // Player's bounding box
    bool onGround;
    float jumpForce;
    float gravity;
    float moveSpeed;

    bool checkCollision(const glm::vec3& newPosition);
    glm::vec3 handleCollision(const glm::vec3& newPosition);
};
