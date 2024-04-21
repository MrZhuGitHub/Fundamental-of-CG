#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace CG {

enum MoveDirection {
    MOVE_FRONT,
    MOVE_BACK,
    MOVE_LEFT,
    MOVE_RIGHT,
};

class camera {
public:
    camera(glm::vec3 cameraPos = glm::vec3(0, 0, 100), glm::vec3 cameraFocus = glm::vec3(0, 0, 0), glm::vec3 cameraUp = glm::vec3(0, 1, 0));

    void move(MoveDirection direction, float distance);

    void viewAngle(float yawOffset, float pitchOffset);

    void zoom(float fovOffset);

    glm::mat4 getViewProjectionMatrix();

private:
    glm::vec3 cameraPos_;
    glm::vec3 cameraFocus_;
    glm::vec3 cameraUp_;
    glm::vec3 cameraDirection_;
    float fov_;
};

}

#endif