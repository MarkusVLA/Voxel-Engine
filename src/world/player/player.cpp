#include "player.h"
#include "glm/fwd.hpp"
#include <algorithm>
#include <iostream>

Player::Player(Camera* camera, ChunkManager* chunkManager)
    : camera(camera), chunkManager(chunkManager),
      size(1.0f, 2.0f, 1.0f), moveSpeed(50.0f) {}

Player::~Player() {
    delete camera;
}

void Player::update(float deltaTime) { /* */ }

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
            movement = glm::vec3(0.0, 1.0, 0.0) * speed;
            break;
        case DOWN:
            movement = glm::vec3(0.0, -1.0, 0.0) * speed;
            break;
        default:
            break;
    }
    glm::vec3 newPosition = camera->getPosition() + movement;
    newPosition = handleCollision(newPosition);
    camera->setPosition(newPosition);
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
                    if (voxel != nullptr && voxel->getType() != WATER) {
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
