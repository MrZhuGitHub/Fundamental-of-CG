#include "curve.h"

#include <string>

namespace CG {
curve::curve(std::vector<vertex> vertices, float* color, float width, CurveType type)
    : vertices_(vertices)
    , posAndSizeMat4_(glm::mat4(1.0f)) {
    width_ = width;
    memcpy(color_, color, 3 * sizeof(float));
    type_ = type;
    setupMesh();
}

void curve::setPosAndSize(glm::mat4 posAndSizeMat4) {
    posAndSizeMat4_ = posAndSizeMat4;
}

void curve::drawCurve(std::shared_ptr<shader> drawShader) {
    drawShader->use();

    // glLineWidth(width);

    // glEnable(GL_BLEND);

    glEnable(GL_LINE_SMOOTH);

    // glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glLineStipple(1, 0x3F3F);

    drawShader->setProperty(color_, "color");

    drawShader->setFloat(width_, "width");

    drawShader->setObjPosMatrix(posAndSizeMat4_);

    glBindVertexArray(VAO_);

    if (type_ == LINE_NO_STRIP) {
        glDrawArrays(GL_LINE_STRIP, 0, vertices_.size());
    } else if (type_ == LINE_STRIP) {
        glDrawArrays(GL_LINES, 0, vertices_.size());
    }
    
    glBindVertexArray(0);
}

curve::~curve() {
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
}

void curve::setupMesh() {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);

    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(vertex), &vertices_[0], GL_STATIC_DRAW);  

    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0);
}
}