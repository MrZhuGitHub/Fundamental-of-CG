#include "curve.h"

#include <string>

namespace CG {
curve::curve(std::vector<vertex> vertices)
    : vertices_(vertices) {
    setupMesh();
}

void curve::drawCurve(std::shared_ptr<shader> drawShader, float* color, float width) {
    drawShader->use();

    // glLineWidth(width);

    // glEnable(GL_BLEND);

    // glEnable(GL_LINE_SMOOTH);

    // glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glLineStipple(1, 0x3F3F);

    drawShader->setProperty(color, "color");

    drawShader->setFloat(width, "width");

    glBindVertexArray(VAO_);

    glDrawArrays(GL_LINES, 0, vertices_.size());
    
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