#include "camera.h"

#include <iostream>

namespace CG {

camera::camera(glm::vec3 cameraPos, glm::vec3 cameraFocus, glm::vec3 cameraUp)
    : cameraPos_(cameraPos)
    , cameraFocus_(cameraFocus)
    , cameraUp_(cameraUp)
    , cameraDirection_(cameraFocus_ - cameraPos_)
    , fov_(45.0f) {

}

void camera::move(MoveDirection direction, float distance) {
    if (MOVE_FRONT == direction) {
        cameraPos_ = cameraPos_ +  glm::normalize(cameraDirection_) * distance;
    } else if (MOVE_BACK == direction) {
        cameraPos_ = cameraPos_ -  glm::normalize(cameraDirection_) * distance;
    } else if (MOVE_LEFT == direction) {
        cameraPos_ = cameraPos_ - glm::normalize(glm::cross(cameraDirection_, cameraUp_)) * distance;
    } else if (MOVE_RIGHT == direction) {
        cameraPos_ = cameraPos_ + glm::normalize(glm::cross(cameraDirection_, cameraUp_)) * distance;
    } else {
        std::cout << "input invalid camera moving direction" << std::endl;
    }
}

void camera::viewAngle(float yawOffset, float pitchOffset) {

    if(pitchOffset > 89.0f)
        pitchOffset = 89.0f;
    if(pitchOffset < -89.0f)
        pitchOffset = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(pitchOffset)) * cos(glm::radians(yawOffset));
    front.y = sin(glm::radians(pitchOffset));
    front.z = cos(glm::radians(pitchOffset)) * sin(glm::radians(yawOffset));
    cameraDirection_ = glm::normalize(front);
}

void camera::zoom(float fovOffset) {
    fov_ = fov_ + fovOffset;
    if (fov_ < 1) {
        fov_ = 1;
    }
    if (fov_ > 80) {
        fov_ = 80;
    }
}

glm::mat4 camera::getViewProjectionMatrix() {
    glm::mat4 view;
    view = glm::lookAt(cameraPos_, cameraPos_ + cameraDirection_, cameraUp_);
    glm::mat4 proj = glm::perspective(glm::radians(fov_), 16.0f/9.0f, 0.1f, 10000.0f);
    return (proj*view);
}

}