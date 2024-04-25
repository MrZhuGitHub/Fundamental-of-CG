#include <glad/glad.h>
#include <GLFW/glfw3.h>
 
#include <iostream>
#include <memory>

#include "model.h"
#include "shader.h"
#include "camera.h"
#include "curve.h"

#include "glm/gtx/string_cast.hpp"

using namespace CG;

#define SCR_WIDTH 1600
#define SCR_HEIGHT 900

std::shared_ptr<model> kModel1, kModel2;
std::shared_ptr<curve> kCurve;
std::shared_ptr<shader> kShader, kCurveShader;
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

    glfwWindowHint(GLFW_SAMPLES, 4);
 
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    kCurveShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/CurvedVertex.glsl",
                                            "/opengles/Fundamental-of-CG/opengl/shader/CurvedFragment.glsl",
                                            "/opengles/Fundamental-of-CG/opengl/shader/CurvedGeometry.glsl");

    float curveColor[] = {0.9f, 0.7f, 0.6f};
    vertex vertex1, vertex2, vertex3, vertex4, vertex5, vertex6;
    vertex1.position = glm::vec3(0.0f, 0.0f, 0.0f);
    vertex2.position = glm::vec3(-100.0f, 0.0f, 0.0f);
    vertex3.position = glm::vec3(-120.0f, 0.0f, 30.0f);
    vertex4.position = glm::vec3(-80.0f, 0.0f, 65.0f);
    vertex5.position = glm::vec3(40.0f, 0.0f, 40.0f);
    vertex6.position = glm::vec3(20.0f, 10.0f, 80.0f);
    std::vector<vertex> curveVertex;
    curveVertex.push_back(vertex1);
    curveVertex.push_back(vertex2);
    curveVertex.push_back(vertex3);
    curveVertex.push_back(vertex4);
    curveVertex.push_back(vertex5);
    curveVertex.push_back(vertex6);
    kCurve = std::make_shared<curve>(curveVertex);

    // kShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/BlinnPhongVertex.glsl",
    //                                    "/opengles/Fundamental-of-CG/opengl/shader/BlinnPhongFragment.glsl");

    // kModel1 = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/mary/Marry.obj");
    // kModel2 = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/floor/floor.obj");    

    kCamera = std::make_shared<camera>();

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    glEnable(GL_DEPTH_TEST);
    
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_MULTISAMPLE);

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
 
        // render
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // kShader->use();

        // kShader->setLight();

        // glm::mat4 defaultModelMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f);

        // kShader->setModelMatrix(defaultModelMatrix);
        // kShader->setViewMatrix(kCamera->getViewMatrix());
        // kShader->setProjectionMatrix(kCamera->getProjectMatrix());

        // kModel1->drawModel(kShader);
        // kModel2->drawModel(kShader);

        kCurveShader->use();

        glm::mat4 defaultModelMatrix = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f);

        kCurveShader->setModelMatrix(defaultModelMatrix);
        kCurveShader->setViewMatrix(kCamera->getViewMatrix());
        kCurveShader->setProjectionMatrix(kCamera->getProjectMatrix());

        kCurve->drawCurve(kCurveShader, curveColor, 8.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    // optional: de-allocate all resources
    glfwTerminate();
    
    return 0;
}
