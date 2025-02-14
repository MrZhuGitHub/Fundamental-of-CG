#include "microfacetBrdf.h"
#include "environmentLight.h"
#include "HierachicalDepthRender.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
 
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>

#include "model.h"
#include "shader.h"
#include "camera.h"
#include "curve.h"
#include "text.h"
#include "framebuffer.h"
#include "texture.h"

#include "glm/gtx/string_cast.hpp"

using namespace CG;

#define SCR_WIDTH 2048
#define SCR_HEIGHT 1024

std::shared_ptr<shader> kShader, kLineShader, kModelShader, kTextShader, kGBufferShader, kIndirectLightShader, kHierachicalDepthShader;
std::shared_ptr<camera> kCamera;
float kReleaseMouseX = 0.0f, kReleaseMouseY = 0.0f;
float kPushMouseX = 0.0f, kPushMouseY = 0.0f;
bool kIfMouseRelease = true;
double mousePosX, mousePosY;

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
    mousePosX = xpos;
    mousePosY = ypos;
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

int renderBev () {
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

    kTextShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/textVertex.glsl",
                                       "/opengles/Fundamental-of-CG/opengl/shader/textFragment.glsl");

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

    //text
    std::shared_ptr<text> textObject = std::make_shared<text>(kTextShader, SCR_WIDTH, SCR_HEIGHT);
    textObject->loadCharacters("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");

    //camera
    kCamera = std::make_shared<camera>();

    //opengl
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    glEnable(GL_DEPTH_TEST);
    
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_MULTISAMPLE);

    // glEnable(GL_CULL_FACE);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

        //draw text
        textObject->renderText("Time: 38748.43s", 10, 850, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
        textObject->renderText("Speed: 108.4km/h", 10, 800, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
        textObject->renderText("Steering: -4.2 deg", 10, 750, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
        textObject->renderText("Yaw Rate: 0.8 deg/s", 10, 700, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));

        //swap frame buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    // optional: de-allocate all resources
    glfwTerminate();
    
    return 0;
}

int renderBasedOnGames202() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    //glfwWindowHint(GLFW_SAMPLES, 4);
 
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GAMES202", NULL, NULL);
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

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //load shader
    kModelShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/pbrVertex.glsl",
                                    "/opengles/Fundamental-of-CG/opengl/shader/pbrFragment.glsl");

    kGBufferShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/GbufferVertex.glsl",
                                       "/opengles/Fundamental-of-CG/opengl/shader/GbufferFragment.glsl");

    kIndirectLightShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/SsrIndirectShadingVertex.glsl",
                                        "/opengles/Fundamental-of-CG/opengl/shader/SsrIndirectShadingFragment.glsl");

    kTextShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/textVertex.glsl",
                                       "/opengles/Fundamental-of-CG/opengl/shader/textFragment.glsl");

    kHierachicalDepthShader = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/HierachicalDepthVertex.glsl",
                                       "/opengles/Fundamental-of-CG/opengl/shader/HierachicalDepthFragment.glsl"); 

    //load model
    std::vector<std::shared_ptr<model>> models;
    auto car = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/911/911.obj");
    glm::mat4 trans1(1.0f);
    trans1 = glm::scale(trans1, glm::vec3(1.0, 1.0, 1.0));
    trans1 = glm::translate(trans1, glm::vec3(-50.0, 0.0, -50.0));
    trans1 = glm::rotate(trans1, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
    trans1 = glm::rotate(trans1, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    car->addInstance(trans1);
    models.push_back(car);

    auto floor = std::make_shared<model>("/opengles/Fundamental-of-CG/opengl/model/floor/floor.obj");
    glm::mat4 trans2(1.0f);
    trans2 = glm::scale(trans2, glm::vec3(7.0, 1.0, 7.0));
    trans2 = glm::translate(trans2, glm::vec3(-10.0, 0.0, 10.0));
    trans2 = glm::rotate(trans2, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    floor->addInstance(trans2);
    models.push_back(floor);

    //camera
    kCamera = std::make_shared<camera>();

    glm::mat4 defaultModelMatrix = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, -3.0f, 3.0f);

    //shadow framebuffer
    std::shared_ptr<frameBuffer> shadowFramebuffer = std::make_shared<frameBuffer>(3*SCR_WIDTH, 3*SCR_HEIGHT);
    shadowFramebuffer->init();

    //light camera
    auto lightCamera = std::make_shared<camera>(glm::vec3(2, 2, -2));
    glm::mat4 lightCameraViewMatrix = lightCamera->getViewMatrix();
    glm::mat4 lightCameraProjectMatrix = glm::ortho(-200.0f,200.0f, -200.0f, 200.0f, 20.0f, 500.0f);
    glm::vec3 lightCameraPosition = lightCamera->getCameraPosition();

    //environment light
    std::map<CubeTextureId, imageFilePath> IBLs = {
        {CUBE_TEXTURE_DOWN,  "/opengles/Fundamental-of-CG/opengl/ibl/sky/down.jpg"},
        {CUBE_TEXTURE_UP,    "/opengles/Fundamental-of-CG/opengl/ibl/sky/up.jpg"},
        {CUBE_TEXTURE_FRONT, "/opengles/Fundamental-of-CG/opengl/ibl/sky/front.jpg"},
        {CUBE_TEXTURE_BACK,  "/opengles/Fundamental-of-CG/opengl/ibl/sky/back.jpg"},
        {CUBE_TEXTURE_LEFT,  "/opengles/Fundamental-of-CG/opengl/ibl/sky/left.jpg"},
        {CUBE_TEXTURE_RIGHT, "/opengles/Fundamental-of-CG/opengl/ibl/sky/right.jpg"},      
    };
    std::shared_ptr<EnvironmentLight> prefilterEnvironmentMap = std::make_shared<EnvironmentLight>(IBLs, 512, 512);
    prefilterEnvironmentMap->preComputerEnvironmentLight(window);
    prefilterEnvironmentMap->createRenderEnvironmentShader();

    //load texture
    std::shared_ptr<textureLoader> EavgPreComputeTexture = std::make_shared<textureLoader>();
    if (!EavgPreComputeTexture->set2Dtexture("/opengles/Fundamental-of-CG/opengl/pbr/EavgImage.png", false)) {
        std::cout << "failed to load EavgPreComputeTexture" << std::endl;
        return -1;
    }
    
    std::shared_ptr<textureLoader> BrdfPreComputeTexture = std::make_shared<textureLoader>();
    if (!BrdfPreComputeTexture->set2Dtexture("/opengles/Fundamental-of-CG/opengl/pbr/MicroModelBrdfImage.png", false)) {
        std::cout << "failed to load BrdfPreComputeTexture" << std::endl;
        return -1;
    }

    //GBuffer
    std::shared_ptr<frameBuffer> geometryBuffer = std::make_shared<frameBuffer>(SCR_WIDTH, SCR_HEIGHT, true);
    geometryBuffer->init();

    //direct shading framebuffer
    std::shared_ptr<frameBuffer> directShadingFramebuffer = std::make_shared<frameBuffer>(SCR_WIDTH, SCR_HEIGHT);
    directShadingFramebuffer->init();

    //indirect shading framebuffer
    std::shared_ptr<frameBuffer> indirectShadingFramebuffer = std::make_shared<frameBuffer>(SCR_WIDTH, SCR_HEIGHT);
    indirectShadingFramebuffer->init();

    std::shared_ptr<HierachicalDepthRender> hierachicalDepthRender = std::make_shared<HierachicalDepthRender>();

    //text
    std::shared_ptr<text> textObject = std::make_shared<text>(kTextShader, SCR_WIDTH, SCR_HEIGHT);
    textObject->loadCharacters("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");

    //opengl
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        {
            geometryBuffer->setup();

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, geometryBuffer->getDepthBuffer(), 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, geometryBuffer->getTexture(), 0);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            kGBufferShader->use();
            kGBufferShader->setModelMatrix(defaultModelMatrix);
            kGBufferShader->setViewMatrix(kCamera->getViewMatrix());
            kGBufferShader->setProjectionMatrix(kCamera->getProjectMatrix());

            kGBufferShader->setBool("enableDepth", true);

            models[0]->drawModel(kGBufferShader);

            glDepthMask(false);

            kGBufferShader->setBool("enableDepth", false);

            models[1]->drawModel(kGBufferShader);

            glDepthMask(true);

            // //draw text
            // textObject->renderText(std::string("X: ") + std::to_string(mousePosX), 10, SCR_HEIGHT - 50, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            // textObject->renderText(std::string("Y: ") + std::to_string(SCR_HEIGHT - mousePosY), 10, SCR_HEIGHT - 100, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            // float pixels[4];
            // geometryBuffer->readPixels(mousePosX, SCR_HEIGHT - mousePosY, 1, 1, pixels);
            // textObject->renderText(std::string("R: ") + std::to_string(pixels[0]), 10, SCR_HEIGHT - 150, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            // textObject->renderText(std::string("G: ") + std::to_string(pixels[1]), 10, SCR_HEIGHT - 200, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            // textObject->renderText(std::string("B: ") + std::to_string(pixels[2]), 10, SCR_HEIGHT - 250, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            // textObject->renderText(std::string("A: ") + std::to_string(pixels[3]), 10, SCR_HEIGHT - 300, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            // float depth;
            // geometryBuffer->readDepth(mousePosX, SCR_HEIGHT - mousePosY, 1, 1, &depth);
            // textObject->renderText(std::string("DEPTH: ") + std::to_string(depth), 10, SCR_HEIGHT - 350, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));

            glDepthFunc(GL_LESS);
            glDepthMask(true);

            int numLevels = 1 + (int)floorf(log2f(fminf(SCR_WIDTH, SCR_HEIGHT)));
            int currentWidth = SCR_WIDTH;
            int currentHeight = SCR_HEIGHT;

            kHierachicalDepthShader->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, geometryBuffer->getDepthBuffer());

            for (int i = 1; i < numLevels; i++) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, geometryBuffer->getDepthBuffer(), i);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, geometryBuffer->getTexture(), i); 

                kHierachicalDepthShader->setInt("u_previousLevel", (i - 1));
                kHierachicalDepthShader->setProperty(glm::ivec2(currentWidth, currentHeight), "u_previousLevelDimensions"); 
            
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, currentWidth, currentHeight);

                hierachicalDepthRender->render();
            }

            geometryBuffer->unload();
            // geometryBuffer->blitToFrameBuffer(0);
        }


        {
            //shadow map
            shadowFramebuffer->setup();
            glViewport(0, 0, 3*SCR_WIDTH, 3*SCR_HEIGHT);
            // render
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //draw model
            kModelShader->use();
            kModelShader->setModelMatrix(defaultModelMatrix);
            kModelShader->setViewMatrix(lightCameraViewMatrix);
            kModelShader->setProjectionMatrix(lightCameraProjectMatrix);

            kModelShader->setBool("shadowMap", true);
            kModelShader->setBool("shadow_enable", true);

            kModelShader->setLight();
            kModelShader->setProperty(lightCameraPosition, "camearPosition");

            for (auto& model : models) {
                model->drawModel(kModelShader);
            }

            shadowFramebuffer->unload();
        }

        {
            directShadingFramebuffer->setup();
            //render environment
            if (prefilterEnvironmentMap) {
                prefilterEnvironmentMap->renderEnvironment(glm::mat4(glm::mat3(kCamera->getViewMatrix())), SCR_WIDTH, SCR_HEIGHT);
            } else {
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            }

            //draw model
            kModelShader->use();
            kModelShader->setModelMatrix(defaultModelMatrix);
            kModelShader->setViewMatrix(kCamera->getViewMatrix());
            kModelShader->setProjectionMatrix(kCamera->getProjectMatrix());

            kModelShader->setLight();
            kModelShader->setProperty(kCamera->getCameraPosition(), "camearPosition");

            //configure shadow map
            kModelShader->setBool("shadowMap", false);
            kModelShader->setBool("shadow_enable", true);
            kModelShader->setProperty(lightCameraProjectMatrix*lightCameraViewMatrix, "shadowMatrix");
            glActiveTexture(GL_TEXTURE0 + shadowFramebuffer->getTexture());
            kModelShader->setInt("shadowTexture", shadowFramebuffer->getTexture());
            glBindTexture(GL_TEXTURE_2D, shadowFramebuffer->getTexture());

            //configure brdf map
            glActiveTexture(GL_TEXTURE0 + EavgPreComputeTexture->getTextureId());
            kModelShader->setInt("EavgTexture", EavgPreComputeTexture->getTextureId());
            glBindTexture(GL_TEXTURE_2D, EavgPreComputeTexture->getTextureId());

            glActiveTexture(GL_TEXTURE0 + BrdfPreComputeTexture->getTextureId());
            kModelShader->setInt("MicroModelBrdfTexture", BrdfPreComputeTexture->getTextureId());
            glBindTexture(GL_TEXTURE_2D, BrdfPreComputeTexture->getTextureId());    

            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterEnvironmentMap->getPreComputerResult()); 
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

            kModelShader->setFloat("maxMipmapLevel", 5.0);    

            kModelShader->setProperty(glm::vec3(0.1, 0.1, 0.1), "modelColor");
            kModelShader->setFloat("roughness", 0.1);
            models[0]->drawModel(kModelShader);

            kModelShader->setProperty(glm::vec3(1.0, 1.0, 1.0), "modelColor");
            kModelShader->setFloat("roughness", 0.0);
            models[1]->drawModel(kModelShader);

            directShadingFramebuffer->unload();
        }

        {
            indirectShadingFramebuffer->setup();

            // render environment
            if (prefilterEnvironmentMap) {
                prefilterEnvironmentMap->renderEnvironment(glm::mat4(glm::mat3(kCamera->getViewMatrix())), SCR_WIDTH, SCR_HEIGHT);
            } else {
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            }

            kIndirectLightShader->use();
            kIndirectLightShader->setModelMatrix(defaultModelMatrix);
            kIndirectLightShader->setViewMatrix(kCamera->getViewMatrix());
            kIndirectLightShader->setProjectionMatrix(kCamera->getProjectMatrix());

            //load GBuffer texture
            glActiveTexture(GL_TEXTURE0 + geometryBuffer->getTexture());
            kIndirectLightShader->setInt("normalMapSampler2D", geometryBuffer->getTexture());
            glBindTexture(GL_TEXTURE_2D, geometryBuffer->getTexture());

            glActiveTexture(GL_TEXTURE0 + geometryBuffer->getDepthBuffer());
            kIndirectLightShader->setInt("depthMapSampler2D", geometryBuffer->getDepthBuffer());
            glBindTexture(GL_TEXTURE_2D, geometryBuffer->getDepthBuffer());

            //load directLight texture
            glActiveTexture(GL_TEXTURE0 + directShadingFramebuffer->getTexture());
            kIndirectLightShader->setInt("directShadingSampler2D", directShadingFramebuffer->getTexture());
            glBindTexture(GL_TEXTURE_2D, directShadingFramebuffer->getTexture());

            
            kIndirectLightShader->setProperty((kCamera->getProjectMatrix()), "camera2screenMatrix");
            kIndirectLightShader->setProperty((kCamera->getViewMatrix()), "world2cameraMatrix");
            kIndirectLightShader->setProperty((kCamera->getProjectMatrix()*kCamera->getViewMatrix()), "world2screenMatrix");
            kIndirectLightShader->setProperty(kCamera->getCameraPosition(), "cameraPosition");
            kIndirectLightShader->setProperty(glm::vec2(SCR_WIDTH, SCR_HEIGHT), "screenResolution");

            kIndirectLightShader->setProperty(glm::vec3(0.1, 0.1, 0.1), "modelColor");
            kIndirectLightShader->setFloat("roughness", 0.1);
            kIndirectLightShader->setBool("SSR", false);
            models[0]->drawModel(kIndirectLightShader);

            kIndirectLightShader->setProperty(glm::vec3(1.0, 1.0, 1.0), "modelColor");
            kIndirectLightShader->setFloat("roughness", 0.0);
            kIndirectLightShader->setBool("SSR", true);
            models[1]->drawModel(kIndirectLightShader);

            //draw text
            textObject->renderText(std::string("X: ") + std::to_string(mousePosX), 10, SCR_HEIGHT - 50, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            textObject->renderText(std::string("Y: ") + std::to_string(SCR_HEIGHT - mousePosY), 10, SCR_HEIGHT - 100, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            float pixels[4];
            indirectShadingFramebuffer->readPixels(mousePosX, SCR_HEIGHT - mousePosY, 1, 1, pixels);
            textObject->renderText(std::string("R: ") + std::to_string(pixels[0]), 10, SCR_HEIGHT - 150, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            textObject->renderText(std::string("G: ") + std::to_string(pixels[1]), 10, SCR_HEIGHT - 200, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            textObject->renderText(std::string("B: ") + std::to_string(pixels[2]), 10, SCR_HEIGHT - 250, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));
            textObject->renderText(std::string("A: ") + std::to_string(pixels[3]), 10, SCR_HEIGHT - 300, 0.6f, glm::vec3(1.0f, 0.0f, 1.0f));

            indirectShadingFramebuffer->unload();
            indirectShadingFramebuffer->blitToFrameBuffer(0);

        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    // optional: de-allocate all resources
    glfwTerminate();
    return 0;
}

void PreComputeMicrofacetBrdf() {
    MicrofacetBRDF::preComputer();
}

int preComputePrefilterEnvironmentMap() {

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
 
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(800, 800, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //glfwWindowHint(GLFW_SAMPLES, 16);
 
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::map<CubeTextureId, imageFilePath> IBLs = {
        {CUBE_TEXTURE_DOWN,  "/opengles/Fundamental-of-CG/opengl/ibl/sky/down.jpg"},
        {CUBE_TEXTURE_UP,    "/opengles/Fundamental-of-CG/opengl/ibl/sky/up.jpg"},
        {CUBE_TEXTURE_FRONT, "/opengles/Fundamental-of-CG/opengl/ibl/sky/front.jpg"},
        {CUBE_TEXTURE_BACK,  "/opengles/Fundamental-of-CG/opengl/ibl/sky/back.jpg"},
        {CUBE_TEXTURE_LEFT,  "/opengles/Fundamental-of-CG/opengl/ibl/sky/left.jpg"},
        {CUBE_TEXTURE_RIGHT, "/opengles/Fundamental-of-CG/opengl/ibl/sky/right.jpg"},      
    };
    std::shared_ptr<EnvironmentLight> prefilterEnvironmentMap = std::make_shared<EnvironmentLight>(IBLs, 512, 512);

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        prefilterEnvironmentMap->preComputerEnvironmentLight(window);
        break;
        glfwPollEvents();
    }

    // optional: de-allocate all resources
    glfwTerminate();

    return 0;
}

int main() {
    //renderBev();
    renderBasedOnGames202();
    //PreComputeMicrofacetBrdf();
    //preComputePrefilterEnvironmentMap();
    return 0;
}
