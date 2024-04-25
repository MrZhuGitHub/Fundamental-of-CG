#ifndef _CURVE_H_
#define _CURVE_H_

#include <glm/glm.hpp>

#include <vector>

#include "model.h"

namespace CG {
class curve {
public:
    curve(std::vector<vertex> vertices);
    void drawCurve(std::shared_ptr<shader> drawShader, float* color, float width);
    ~curve();

private:
    void setupMesh();

private:
    std::vector<vertex> vertices_;
    unsigned int VAO_;
    unsigned int VBO_;
};
}


#endif