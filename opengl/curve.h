#ifndef _CURVE_H_
#define _CURVE_H_

#include <glm/glm.hpp>

#include <vector>

#include "model.h"

enum CurveType {
    LINE_NO_STRIP,
    LINE_STRIP,
    CURVE_NO_STRIP,
    curve_STRIP,
};

namespace CG {
class curve {
public:
    curve(std::vector<vertex> vertices, float* color, float width, CurveType type);
    void drawCurve(std::shared_ptr<shader> drawShader);
    void setPosAndSize(glm::mat4 posAndSizeMat4);
    void moveLeft(float distance) {
        posAndSizeMat4_[3][0] = posAndSizeMat4_[3][0] - distance;
    }
    void moveRight(float distance) {
        posAndSizeMat4_[3][0] = posAndSizeMat4_[3][0] + distance;
    }
    void moveForward(float distance) {
        posAndSizeMat4_[3][2] = posAndSizeMat4_[3][2] - distance;
    }
    void moveBack(float distance) {
        posAndSizeMat4_[3][2] = posAndSizeMat4_[3][2] + distance;
    }
    ~curve();

private:
    void setupMesh();

private:
    std::vector<vertex> vertices_;
    float color_[3];
    float width_;
    unsigned int VAO_;
    unsigned int VBO_;
    CurveType type_;
    glm::mat4 posAndSizeMat4_;
};
}


#endif