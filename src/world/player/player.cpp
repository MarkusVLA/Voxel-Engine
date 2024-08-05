#include "player.h"
#include "glm/fwd.hpp"
#include <algorithm>
#include <iostream>

Player::Player(Camera* camera, ChunkManager* chunkManager)
    : camera(camera), chunkManager(chunkManager),
      size(1.0f, 3.0f, 1.0f), moveSpeed(10.0f),
      velocity(0.0f), isOnGround(false),
      gravity(-20.8f), jumpForce(7.0f) {}

Player::~Player() {
    delete camera;
}

void Player::update(float deltaTime) {
    applyGravity(deltaTime);
    updatePosition(deltaTime);
}

void Player::applyGravity(float deltaTime) {
    velocity.y += gravity * deltaTime;
}

void Player::updatePosition(float deltaTime) {
    glm::vec3 newPosition = camera->getPosition() + velocity * deltaTime;
    glm::vec3 finalPosition = handleCollision(newPosition);

    isOnGround = (finalPosition.y == camera->getPosition().y && velocity.y < 0);

    if (isOnGround) {
        velocity.y = 0;
    }

    camera->setPosition(finalPosition);
}

void Player::move(PlayerMovement direction, float deltaTime) {
    float speed = moveSpeed * deltaTime;
    glm::vec3 movement(0.0f);

    switch (direction) {
        case FORWARD:
            movement = camera->getFront() * speed;
            break;
        case BACKWARD:
            movement = -camera->getFront() * speed;
            break;
        case LEFT:
            movement = -camera->getRight() * speed;
            break;
        case RIGHT:
            movement = camera->getRight() * speed;
            break;
        case UP:
            if (isOnGround) {
                velocity.y = jumpForce;
                isOnGround = false;
            }
            return;
        case DOWN:
            return;

        default:
            return;
    }

    // Remove vertical component from movement for walking
    movement.y = 0;

    // Apply movement directly to position
    glm::vec3 newPosition = camera->getPosition() + movement;
    glm::vec3 finalPosition = handleCollision(newPosition);
    camera->setPosition(finalPosition);
}

glm::vec3 Player::getPosition() const {
    return camera->getPosition();
}

Camera* Player::getCamera() {
    return camera;
}

bool Player::checkCollision(const glm::vec3& position) {
    glm::vec3 minBound = position - size * 0.5f;
    glm::vec3 maxBound = position + size * 0.5f;

    for (int x = floor(minBound.x); x <= floor(maxBound.x); x++) {
        for (int y = floor(minBound.y); y <= floor(maxBound.y); y++) {
            for (int z = floor(minBound.z); z <= floor(maxBound.z); z++) {
                glm::ivec2 chunkPos(floor(float(x) / 16.0f), floor(float(z) / 16.0f));
                glm::ivec3 localPos(x & 15, y, z & 15);  // Convert to chunk-local coordinates

                std::shared_ptr<Chunk> chunk = chunkManager->getChunk(chunkPos);
                if (chunk) {
                    Voxel* voxel = chunk->getVoxel(localPos);
                    if (voxel != nullptr && voxel->getType() != WATER) { // Handle also tall grass
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

glm::vec3 Player::handleCollision(const glm::vec3& newPosition) {
    glm::vec3 currentPosition = camera->getPosition();
    glm::vec3 movement = newPosition - currentPosition;
    glm::vec3 finalPosition = currentPosition;

    // Check and update X axis
    glm::vec3 testPosition = currentPosition + glm::vec3(movement.x, 0, 0);
    if (!checkCollision(testPosition)) {
        finalPosition.x = testPosition.x;
    }

    // Check and update Y axis
    testPosition = finalPosition + glm::vec3(0, movement.y, 0);
    if (!checkCollision(testPosition)) {
        finalPosition.y = testPosition.y;
    }

    // Check and update Z axis
    testPosition = finalPosition + glm::vec3(0, 0, movement.z);
    if (!checkCollision(testPosition)) {
        finalPosition.z = testPosition.z;
    }

    return finalPosition;
}
