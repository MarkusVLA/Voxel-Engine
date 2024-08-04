#ifndef CAMERA_H
#define CAMERA_H

#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
    glm::mat4 getViewMatrix();
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);
    glm::vec3 getRayDirection() const;
    float getZoom();
    glm::vec3 getPosition();
    glm::vec3 getFront();
    glm::vec3 getRight();
    void setPosition(glm::vec3 pos);

private:
    void updateCameraVectors();
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float yaw;
    float pitch;
    float mouseSensitivity;
    float zoom;
};

#endif // CAMERA_H
