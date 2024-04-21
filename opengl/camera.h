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
    camera(glm::vec3 cameraPos, glm::vec3 cameraFocus, glm::vec3 cameraUp);

    void move(MoveDirection direction, double distance);

    void viewAngle(double yawOffset, double pitchOffset);

    void zoom(double fovOffset);

    glm::mat4 getViewProjectionMatrix();

private:
    glm::vec3 cameraPos_;
    glm::vec3 cameraFocus_;
    glm::vec3 cameraUp_;
    glm::vec3 cameraDirection_;
    double fov_;
}

}

#endif