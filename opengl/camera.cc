#include "camera.h"

#include <iostream>

namespace CG {

camera::camera(glm::vec3 cameraPos, glm::vec3 cameraFocus, glm::vec3 cameraUp)
    : cameraPos_(cameraPos)
    , cameraFocus_(cameraFocus)
    , cameraUp_(cameraUp)
    , cameraDirection_(cameraFocus_ - cameraPos_)
    , fov_(45.0f)
    , yaw_(90.0f)
    , pitch_(0.0f) {

}

void camera::move(MoveDirection direction, float distance) {
    glm::vec3 x(cameraPos_.x, 0, cameraPos_.z);
    x = glm::normalize(x);
    glm::vec3 y = glm::cross(x, glm::vec3(0, 1, 0));
    y = glm::normalize(y);
    if (MOVE_FRONT == direction) {
        cameraFocus_ = cameraFocus_ - x * distance;
    } else if (MOVE_BACK == direction) {
        cameraFocus_ = cameraFocus_ + x * distance;
    } else if (MOVE_LEFT == direction) {
        cameraFocus_ = cameraFocus_ + y * distance;
    } else if (MOVE_RIGHT == direction) {
        cameraFocus_ = cameraFocus_ - y * distance;
    } else {
        std::cout << "input invalid camera moving direction" << std::endl;
    }
}

void camera::viewAngle(float yawOffset, float pitchOffset) {

    pitch_ = pitch_ + pitchOffset;
    yaw_ = yaw_ + yawOffset;
    if(pitch_ > 89.0f)
        pitch_ = 89.0f;
    if(pitch_ < -89.0f)
        pitch_ = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
    front.y = sin(glm::radians(pitch_));
    front.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));
    cameraPos_ = glm::normalize(front);
}

void camera::zoom(float fovOffset) {
    fov_ = fov_ + fovOffset;
    if (fov_ < 1) {
        fov_ = 1;
    }
}

glm::mat4 camera::getViewMatrix() {
    glm::mat4 view;
    view = glm::lookAt(fov_*cameraPos_ + cameraFocus_, cameraFocus_, cameraUp_);
    return view;
}

glm::mat4 camera::getProjectMatrix() {
    glm::mat4 proj = glm::perspective(glm::radians(30.0f), 9.0f/9.0f, 0.01f, 1000.0f);
    return proj;
}

}