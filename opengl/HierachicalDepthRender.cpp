#include "HierachicalDepthRender.h"
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>

namespace CG {

HierachicalDepthRender::HierachicalDepthRender() {
    float quad[] = {
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 
        1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, 
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 
    };

    glGenBuffers(1, &VBO_);
    glGenVertexArrays(1, &VAO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);

    glBufferData(GL_ARRAY_BUFFER, 6*8*sizeof(GL_FLOAT), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (void*)0);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (void*)0 + 3 * sizeof(float));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (void*)0 + 6 * sizeof(float));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

HierachicalDepthRender::~HierachicalDepthRender() {

}

void HierachicalDepthRender::render() {
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(VAO_);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

}