#include <glad/glad.h>
#include <GLFW/glfw3.h>
 
#include <iostream>
#include <memory>
#include <vector>

#include "model.h"
#include "shader.h"
#include "camera.h"
#include "curve.h"

#include "glm/gtx/string_cast.hpp"

using namespace CG;

#define SCR_WIDTH 1600
#define SCR_HEIGHT 900

std::shared_ptr<shader> kLineShader, kModelShader;
std::shared_ptr<camera> kCamera;
float kReleaseMouseX = 0.0f, kReleaseMouseY = 0.0f;
float kPushMouseX = 0.0f, kPushMouseY = 0.0f;
bool kIfMouseRelease = true;

void processInput(GLFWwindow *window)
{
    float cameraMoveSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        kCamera->move(MOVE_FRONT, cameraMoveSpeed);
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        kCamera->move(MOVE_BACK, cameraMoveSpeed);
    } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        kCamera->move(MOVE_LEFT, cameraMoveSpeed);
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        kCamera->move(MOVE_RIGHT, cameraMoveSpeed);
    }
}
 
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        kPushMouseX = xpos;
        kPushMouseY = ypos;
        kIfMouseRelease = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        if (false == kIfMouseRelease) {
            float xoffset = xpos - kReleaseMouseX;
            float yoffset = ypos - kReleaseMouseY;
            float sensitivity = 0.1;
            xoffset *= sensitivity;
            yoffset *= sensitivity;
            kCamera->viewAngle(xoffset, yoffset);
        }
        kReleaseMouseX = xpos;
        kReleaseMouseY = ypos;
        kIfMouseRelease = true;
    }    
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    kCamera->zoom(yoffset);
}

std::vector<vertex> generateLine() {
    std::vector<vertex> vertexes;
    for (int i = 0; i < 200; i++) {
        vertex v;
        v.position.x = 0;
        v.position.y = 0;
        v.position.z = 0 - i;
        vertexes.push_back(v);
    }
    return vertexes;
}

int main () {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
 
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwWindowHint(GLFW_SAMPLES, 16);
 
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //shader
    kLineShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/CurvedVertex.glsl",
                                            "/opengles/Fundamental-of-CG/opengl/shader/CurvedFragment.glsl",
                                            "/opengles/Fundamental-of-CG/opengl/shader/CurvedGeometry.glsl");

    kModelShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/BlinnPhongVertex.glsl",
                                       "/opengles/Fundamental-of-CG/opengl/shader/BlinnPhongFragment.glsl");

    
    //model
    std::vector<std::shared_ptr<model>> models;
    // auto car = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/object/911gt3.STL");
    // models.push_back(car);
    auto mary = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/mary/Marry.obj");
    models.push_back(mary);

    std::vector<std::shared_ptr<curve>> lines;
    // std::vector<vertex> vertexes = generateLine();
    // float yellow[3] = {1.0f, 1.0f, 0.0f};
    // float white[3] = {1.0f, 1.0f, 1.0f};
    // auto line1 = std::make_shared<curve>(vertexes, white, 0.1, CurveType::LINE_NO_STRIP);
    // line1->moveLeft(8.0);
    // auto line2 = std::make_shared<curve>(vertexes, white, 0.1, CurveType::LINE_STRIP);
    // line2->moveLeft(4.0);
    // auto line3 = std::make_shared<curve>(vertexes, yellow, 0.1, CurveType::LINE_NO_STRIP);
    // auto line4 = std::make_shared<curve>(vertexes, white, 0.1, CurveType::LINE_STRIP);
    // line4->moveRight(4.0);
    // auto line5 = std::make_shared<curve>(vertexes, white, 0.1, CurveType::LINE_NO_STRIP);
    // line5->moveRight(8.0);
    // lines.push_back(line1);
    // lines.push_back(line2);
    // lines.push_back(line3);
    // lines.push_back(line4);
    // lines.push_back(line5);

    //camera
    kCamera = std::make_shared<camera>();

    //opengl
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    glEnable(GL_DEPTH_TEST);
    
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_MULTISAMPLE);

    glm::mat4 defaultModelMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f);

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
 
        // render
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw model
        kModelShader->use();
        kModelShader->setModelMatrix(defaultModelMatrix);
        kModelShader->setViewMatrix(kCamera->getViewMatrix());
        kModelShader->setProjectionMatrix(kCamera->getProjectMatrix());

        for (auto& model : models) {
            model->drawModel(kModelShader);
        }

        // //draw line
        // kLineShader->use();
        // kLineShader->setModelMatrix(defaultModelMatrix);
        // kLineShader->setViewMatrix(kCamera->getViewMatrix());
        // kLineShader->setProjectionMatrix(kCamera->getProjectMatrix());

        // for (auto& line : lines) {
        //     line->drawCurve(kLineShader);
        // }

        //swap frame buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    // optional: de-allocate all resources
    glfwTerminate();
    
    return 0;
}
