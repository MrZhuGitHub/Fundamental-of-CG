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

std::shared_ptr<shader> kShader, kLineShader, kModelShader;
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
    
    kShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/vertex.glsl",
                                       "/opengles/Fundamental-of-CG/opengl/shader/fragment.glsl");

    //model
    std::vector<std::shared_ptr<model>> models;
    auto car = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/object/TeslaModel/TeslaModel.obj");
    glm::mat4 trans1(1.0f);
    trans1 = glm::scale(trans1, glm::vec3(0.033, 0.033, 0.033));
    trans1 = glm::translate(trans1, glm::vec3(60.0, 0.0, -5000.0));
    car->addInstance(trans1);

    glm::mat4 trans5(1.0f);
    trans5 = glm::scale(trans5, glm::vec3(0.033, 0.033, 0.033));
    trans5 = glm::translate(trans5, glm::vec3(180.0, 0.0, -5300.0));
    car->addInstance(trans5);

    glm::mat4 trans6(1.0f);
    trans6 = glm::scale(trans6, glm::vec3(0.033, 0.033, 0.033));
    trans6 = glm::translate(trans6, glm::vec3(120.0, 0.0, -4000.0));
    car->addInstance(trans6);

    glm::mat4 trans7(1.0f);
    trans7 = glm::scale(trans7, glm::vec3(0.033, 0.033, 0.033));
    trans7 = glm::translate(trans7, glm::vec3(-60.0, 0.0, -4300.0));
    trans7 = glm::rotate(trans7, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
    car->addInstance(trans7);

    glm::mat4 trans8(1.0f);
    trans8 = glm::scale(trans8, glm::vec3(0.033, 0.033, 0.033));
    trans8 = glm::translate(trans8, glm::vec3(-80.0, 0.0, -5500.0));
    trans8 = glm::rotate(trans8, glm::radians(185.0f), glm::vec3(0.0, 1.0, 0.0));
    car->addInstance(trans8);
    models.push_back(car);

    auto people = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/people.STL");
    glm::mat4 trans2(1.0f);
    trans2 = glm::scale(trans2, glm::vec3(0.002, 0.002, 0.002));
    trans2 = glm::translate(trans2, glm::vec3(87.0*60.0, 0.0, 16.0*-5000.0));
    trans2 = glm::rotate(trans2, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
    people->addInstance(trans2);

    glm::mat4 trans21(1.0f);
    trans21 = glm::scale(trans21, glm::vec3(0.002, 0.002, 0.002));
    trans21 = glm::translate(trans21, glm::vec3(84.0*60.0, 0.0, 16.5*-5000.0));
    trans21 = glm::rotate(trans21, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0));
    people->addInstance(trans21);

    glm::mat4 trans22(1.0f);
    trans22 = glm::scale(trans22, glm::vec3(0.002, 0.002, 0.002));
    trans22 = glm::translate(trans22, glm::vec3(80.0*60.0, 0.0, 17.0*-5000.0));
    trans22 = glm::rotate(trans22, glm::radians(-10.0f), glm::vec3(0.0, 1.0, 0.0));
    people->addInstance(trans22);

    glm::mat4 trans23(1.0f);
    trans23 = glm::scale(trans23, glm::vec3(0.002, 0.002, 0.002));
    trans23 = glm::translate(trans23, glm::vec3(87.0*60.0, 0.0, 15.0*-5000.0));
    trans23 = glm::rotate(trans23, glm::radians(2.0f), glm::vec3(0.0, 1.0, 0.0));
    people->addInstance(trans23);

    glm::mat4 trans24(1.0f);
    trans24 = glm::scale(trans24, glm::vec3(0.002, 0.002, 0.002));
    trans24 = glm::translate(trans24, glm::vec3(90*60.0, 0.0, 15.6*-5000.0));
    trans24 = glm::rotate(trans24, glm::radians(-3.0f), glm::vec3(0.0, 1.0, 0.0));
    people->addInstance(trans24);
    models.push_back(people);

    auto bus = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/bus.STL");
    glm::mat4 trans3(1.0f);
    trans3 = glm::scale(trans3, glm::vec3(0.001, 0.001, 0.001));
    trans3 = glm::translate(trans3, glm::vec3(-67.0*60.0, 0.0, 30.0*-5000.0));
    trans3 = glm::rotate(trans3, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
    bus->addInstance(trans3);

    glm::mat4 trans31(1.0f);
    trans31 = glm::scale(trans31, glm::vec3(0.001, 0.001, 0.001));
    trans31 = glm::translate(trans31, glm::vec3(33.0*60.0, 0.0, 30.0*-3000.0));
    trans31 = glm::rotate(trans31, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
    bus->addInstance(trans31);
    models.push_back(bus);

    auto bicycle = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/bicycle.STL");
    glm::mat4 trans4(1.0f);
    trans4 = glm::scale(trans4, glm::vec3(0.002, 0.002, 0.002));
    trans4 = glm::translate(trans4, glm::vec3(85.0*60.0, 0.0, 16.0*-5000.0));
    trans4 = glm::rotate(trans4, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    bicycle->addInstance(trans4);

    glm::mat4 trans41(1.0f);
    trans41 = glm::scale(trans41, glm::vec3(0.002, 0.002, 0.002));
    trans41 = glm::translate(trans41, glm::vec3(75.0*60.0, 0.0, 15.7*-5000.0));
    trans41 = glm::rotate(trans41, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    bicycle->addInstance(trans41);

    glm::mat4 trans42(1.0f);
    trans42 = glm::scale(trans42, glm::vec3(0.002, 0.002, 0.002));
    trans42 = glm::translate(trans42, glm::vec3(80.0*60.0, 0.0, 15.0*-5000.0));
    trans42 = glm::rotate(trans42, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    bicycle->addInstance(trans42);
    models.push_back(bicycle);

    //arrow
    std::vector<std::shared_ptr<model>> arrows;
    auto arrow = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/arrow.STL");
    glm::mat4 trans61(1.0f);
    trans61 = glm::scale(trans61, glm::vec3(3.0, 3.0, 3.0));
    trans61 = glm::translate(trans61, glm::vec3(0.7, 0.0, -53));
    trans61 = glm::rotate(trans61, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
    arrow->addInstance(trans61);

    glm::mat4 trans62(1.0f);
    trans62 = glm::scale(trans62, glm::vec3(3.0, 3.0, 3.0));
    trans62 = glm::translate(trans62, glm::vec3(2.1, 0.0, -53));
    trans62 = glm::rotate(trans62, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
    arrow->addInstance(trans62);

    glm::mat4 trans63(1.0f);
    trans63 = glm::scale(trans63, glm::vec3(3.0, 3.0, 3.0));
    trans63 = glm::translate(trans63, glm::vec3(-0.7, 0.0, -50));
    trans63 = glm::rotate(trans63, glm::radians(-180.0f), glm::vec3(0.0, 1.0, 0.0));
    arrow->addInstance(trans63);

    glm::mat4 trans64(1.0f);
    trans64 = glm::scale(trans64, glm::vec3(3.0, 3.0, 3.0));
    trans64 = glm::translate(trans64, glm::vec3(-2.1, 0.0, -50));
    trans64 = glm::rotate(trans64, glm::radians(-180.0f), glm::vec3(0.0, 1.0, 0.0));
    arrow->addInstance(trans64);
    arrows.push_back(arrow);

    //line
    std::vector<std::shared_ptr<curve>> lines;
    std::vector<vertex> vertexes = generateLine();
    float yellow[3] = {1.0f, 1.0f, 0.0f};
    float white[3] = {1.0f, 1.0f, 1.0f};
    auto line1 = std::make_shared<curve>(vertexes, white, 0.1, CurveType::LINE_NO_STRIP);
    line1->moveLeft(8.0);
    auto line2 = std::make_shared<curve>(vertexes, white, 0.1, CurveType::LINE_STRIP);
    line2->moveLeft(4.0);
    auto line3 = std::make_shared<curve>(vertexes, yellow, 0.1, CurveType::LINE_NO_STRIP);
    auto line4 = std::make_shared<curve>(vertexes, white, 0.1, CurveType::LINE_STRIP);
    line4->moveRight(4.0);
    auto line5 = std::make_shared<curve>(vertexes, white, 0.1, CurveType::LINE_NO_STRIP);
    line5->moveRight(8.0);
    lines.push_back(line1);
    lines.push_back(line2);
    lines.push_back(line3);
    lines.push_back(line4);
    lines.push_back(line5);

    //camera
    kCamera = std::make_shared<camera>();

    //opengl
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    glEnable(GL_DEPTH_TEST);
    
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_MULTISAMPLE);

    glm::mat4 defaultModelMatrix = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, -3.0f, 3.0f);

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

        kModelShader->setLight();

        for (auto& model : models) {
            model->drawModel(kModelShader);
        }

        //draw line
        kLineShader->use();
        kLineShader->setModelMatrix(defaultModelMatrix);
        kLineShader->setViewMatrix(kCamera->getViewMatrix());
        kLineShader->setProjectionMatrix(kCamera->getProjectMatrix());

        for (auto& line : lines) {
            line->drawCurve(kLineShader);
        }

        //draw arrow
        kShader->use();
        kShader->setModelMatrix(defaultModelMatrix);
        kShader->setViewMatrix(kCamera->getViewMatrix());
        kShader->setProjectionMatrix(kCamera->getProjectMatrix());   

        for (auto& arrow : arrows) {
            arrow->drawModel(kShader);
        }

        //swap frame buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    // optional: de-allocate all resources
    glfwTerminate();
    
    return 0;
}
