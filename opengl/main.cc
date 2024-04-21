#include <glad/glad.h>
#include <GLFW/glfw3.h>
 
#include <iostream>
#include <memory>

#include "model.h"
#include "shader.h"
#include "camera.h"

using namespace CG;

#define SCR_WIDTH 1600
#define SCR_HEIGHT 900

std::shared_ptr<model> kModel;
std::shared_ptr<shader> kShader;
std::shared_ptr<camera> kCamera;
float lastX = SCR_WIDTH, lastY = SCR_HEIGHT;
bool firstMouse = true;

//键盘按键回调函数  
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
 
//调整窗口大小回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    kCamera->viewAngle(xoffset, yoffset);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
 
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    kShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/BlinnPhongVertex.glsl",
                                       "/opengles/Fundamental-of-CG/opengl/shader/BlinnPhongFragment.glsl");

    kModel = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/mary/Marry.obj");

    kCamera = std::make_shared<camera>();

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
 
        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
 
        kShader->setViewProjectionMatrix(kCamera->getViewProjectionMatrix());
        kModel->drawModel(kShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    // optional: de-allocate all resources
    glfwTerminate();
    
    return 0;
}
