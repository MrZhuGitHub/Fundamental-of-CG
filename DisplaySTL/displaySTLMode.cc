//g++ -g -o displaySTLMode displaySTLMode.cc -lEGL -lGLESv2 -lX11

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <iostream>
#include <string.h>
#include <memory>
#include <cstdlib>
#include <stdlib.h>
#include <thread>
#include <chrono>

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

#include "../Common/esUtil.h"
#include "../Common/loadStlMode.hpp"
#include "../Common/MvpTransform.hpp"

#define WINDOW_WIDTH 1960
#define WINDOW_HIGHT 1080

static Display *x_display = NULL;

static std::vector<Vertex> kVertexs;

static float kRotationY = 0;

GLuint LoadShader(GLenum type, const char* shaderSrc)
{
   GLuint shader;
   GLint compiled;

   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
   {
      return 0;
   }

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );

   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled )
   {
      glDeleteShader ( shader );
      return 0;
   }

   return shader;
}

bool createNativeWindow(uint32_t width, uint32_t height, EGLNativeWindowType& nativeWindow) {
    Window root;
    XSetWindowAttributes swa;
    XSetWindowAttributes  xattr;
    Atom wm_state;
    XWMHints hints;
    XEvent xev;
    EGLConfig ecfg;
    EGLint num_config;
    Window win;

    /*
     * X11 native display initialization
     */

    x_display = XOpenDisplay(NULL);
    if ( x_display == NULL )
    {
        return false;
    }

    root = DefaultRootWindow(x_display);

    swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;
    win = XCreateWindow(
               x_display, root,
               0, 0, width, height, 0,
               CopyFromParent, InputOutput,
               CopyFromParent, CWEventMask,
               &swa );

    xattr.override_redirect = FALSE;
    XChangeWindowAttributes ( x_display, win, CWOverrideRedirect, &xattr );

    hints.input = TRUE;
    hints.flags = InputHint;
    XSetWMHints(x_display, win, &hints);

    // make the window visible on the screen
    XMapWindow (x_display, win);
    XStoreName (x_display, win, "zhuhui");

    // get identifiers for the provided atom name strings
    wm_state = XInternAtom (x_display, "_NET_WM_STATE", FALSE);

    memset ( &xev, 0, sizeof(xev) );
    xev.type                 = ClientMessage;
    xev.xclient.window       = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 1;
    xev.xclient.data.l[1]    = FALSE;
    XSendEvent (
       x_display,
       DefaultRootWindow ( x_display ),
       FALSE,
       SubstructureNotifyMask,
       &xev );

    nativeWindow = (EGLNativeWindowType) win;
    return true;
}

bool init(EGLDisplay& display, EGLSurface& eglSurface){
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (EGL_NO_DISPLAY == display) {
        std::cout << "eglGetDisplay do failed" << std::endl;
        return false;
    }

    EGLint majorVersion, minorVersion;
    if (!eglInitialize(display, &majorVersion, &minorVersion)) {
        std::cout << "eglInitialize do failed" << std::endl;
        return false;       
    }

   EGLint eglConfigAttribList[] =
   {
       EGL_RED_SIZE,       5,
       EGL_GREEN_SIZE,     6,
       EGL_BLUE_SIZE,      5,
       EGL_ALPHA_SIZE,     (ES_WINDOW_RGB & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
       EGL_DEPTH_SIZE,     8,
       /*
       Below configuration make EGL_DEPTH_SIZE equal to zero, which make opengles depth test invalid.
       EGL_DEPTH_SIZE,     (ES_WINDOW_RGB & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
       */
       EGL_STENCIL_SIZE,   (ES_WINDOW_RGB & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
       EGL_SAMPLE_BUFFERS, (ES_WINDOW_RGB & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
       EGL_NONE
   };


    EGLConfig eglConfig;
    EGLint suitableConfig;
    if (!eglChooseConfig(display, eglConfigAttribList, &eglConfig, 1, &suitableConfig)) {
        std::cout << "eglChooseConfig do failed" << std::endl;
        return false;      
    }

    EGLNativeWindowType nativeWindow;
    if (!createNativeWindow(WINDOW_WIDTH, WINDOW_HIGHT, nativeWindow)) {
        std::cout << "createNativeWindow do failed" << std::endl;
        return false;      
    }

    eglSurface = eglCreateWindowSurface(display, eglConfig, nativeWindow, NULL);
    if (EGL_NO_SURFACE == eglSurface) {
        std::cout << "eglCreateWindowSurface do failed" << std::endl;
        return false;             
    }

    EGLint eglContextAttribList[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
    EGLContext eglContext = eglCreateContext(display, eglConfig, EGL_NO_CONTEXT, eglContextAttribList);
    if (EGL_NO_CONTEXT == eglContext) {
        std::cout << "eglCreateContext do failed" << std::endl;
        return false;   
    }

    if (eglMakeCurrent(display, eglSurface, eglSurface, eglContext)) {
        std::cout << "create windows success!" << std::endl;
        return true;
    } else {
        std::cout << "eglMakeCurrent do failed" << std::endl;
        return false;   
    }
};

bool createGlesProgram(GLuint& glesProgram) {
    std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    char vertexShaderStr[] = 
        "precision highp float; \n"
        "precision highp int; \n"
        "attribute vec4 vPosition;  \n"
        "attribute vec3 vNormal;  \n"
        "uniform mat4 modelMatrix; \n"
        "uniform mat4 viewMatrix; \n"
        "uniform mat4 projectionMatrix; \n"
        "uniform mat4 normalMatrix; \n"
        "uniform vec3 lightPosition; \n"
        "varying vec4 normal; \n"
        "varying vec3 halfLightView; \n"
        "varying vec3 lightDirection; \n"
        "uniform float x; \n"
        "uniform float y; \n"
        "uniform float z; \n"
        "uniform float homogeneous; \n"
        "void main()    \n"
        "{  \n"
        "   vec4 vVerticPositionInCameraCoordinate = viewMatrix * modelMatrix * vPosition;   \n"
        //"   vec4 vNormalInCameraCoordinate = normalMatrix * vec4(vNormal, 0.0);   \n"
        "   vec4 vNormalInCameraCoordinate = viewMatrix * (vec4(vNormal, 0.0) + modelMatrix * vPosition) - vVerticPositionInCameraCoordinate; \n"
        "   normal = vNormalInCameraCoordinate;   \n"
        "   vec4 vLightPositionInCameraCoordinate = viewMatrix * vec4(lightPosition, 1.0);   \n"
        "   lightDirection = normalize(vLightPositionInCameraCoordinate.xyz - vVerticPositionInCameraCoordinate.xyz);   \n"
        "   vec3 viewDirection = normalize(-vVerticPositionInCameraCoordinate.xyz);   \n"
        "   halfLightView = normalize(viewDirection + lightDirection);   \n"
        "   gl_Position = projectionMatrix * vVerticPositionInCameraCoordinate;   \n"
        "}  \n";

    char fragmentShaderStr[] =
      "precision highp float; \n"
      "varying vec4 normal; \n"
      "varying vec3 halfLightView; \n"
      "varying vec3 lightDirection; \n"
      "uniform vec3 intensity; \n"
      "uniform vec3 Ia; \n"
      "uniform vec3 ka; \n"
      "uniform vec3 kd; \n"
      "uniform vec3 ks; \n"
      "uniform float phongExp; \n"
      "void main()   \n"
      "{   \n"
      "  vec3 n = normalize(normal.xyz);   \n"
      "  vec3 l = normalize(lightDirection);   \n"
      "  vec3 h = normalize(halfLightView);   \n"
      "  vec3 shadow = ka*Ia;   \n"
      "  vec3 diffuse = vec3(0.0, 0.0, 0.0);   \n"
      "  if (dot(l, n) >= 0.0) {   \n"
      "     diffuse = kd*intensity*dot(l, n);   \n"
      "  }\n"
      "  vec3 phong = vec3(0.0, 0.0, 0.0);   \n"
      "  if (dot(h, n) >= 0.0) {   \n"
      "     phong = ks*intensity*pow(dot(h, n), phongExp);  \n"
      "  }   \n"    
      "  vec3 color = shadow + diffuse + phong;   \n"
      "  gl_FragColor = vec4(color, 1.0);   \n"
      "}   \n";

    auto vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderStr);
    if (0 == vertexShader) {
        std::cout << "load vertex shader failed." << std::endl;
        return false;
    }

    auto fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentShaderStr);
    if (0 == fragmentShader) {
        std::cout << "load fragment shader failed." << std::endl;
        return false;
    }

    glesProgram = glCreateProgram();
    if (0 == glesProgram) {
        std::cout << "glCreateProgram return failed" << std::endl;
    }

    glAttachShader(glesProgram, vertexShader);
    glAttachShader(glesProgram, fragmentShader);

    glLinkProgram(glesProgram);
    GLint linkStatus;
    glGetProgramiv(glesProgram, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
        std::cout << "glLinkProgram return failed" << std::endl;
        glDeleteProgram(glesProgram);
        return false;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return true;
}

void draw(GLuint& glesProgram, float vertex[], GLint vectexCount) {
    glBindAttribLocation(glesProgram, 0, "vPosition");
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 7*sizeof(GL_FLOAT), (void*)(vertex));
    glEnableVertexAttribArray(0);

    glBindAttribLocation(glesProgram, 1, "vNormal");
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 7*sizeof(GL_FLOAT), (void*)(vertex + 4));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, vectexCount);
};

bool LoadModelVertex(const char* path, float** vectex, int& vectexCount, TransformMatrix& modelTransformMatrix) {
    LoadStlMode loader(path);
    if (!loader.LoadTriangleVertex())
        return false;

    std::vector<Vertex> vertexs = loader.getTriangleVertexs();
    kVertexs = vertexs;
    (*vectex) = (float*)(new char[vertexs.size()*sizeof(Vertex)]());
    memcpy((*vectex), vertexs.data(), vertexs.size()*sizeof(Vertex));

    vectexCount = vertexs.size();

    // for (auto& it : vertexs) {
    //     std::cout << it.xPosition << ',' << it.yPosition << ',' << it.zPosition << "\t";
    //     std::cout << it.xNormal << ',' << it.yNormal << ',' << it.zNormal << std::endl;
    // }

    VertexProperty property;
    if (!loader.getProperty(property))
        return false;
    modelTransformMatrix(0, 0) = 1;
    modelTransformMatrix(0, 3) = -std::get<0>(property.center);
    modelTransformMatrix(1, 1) = 1;
    modelTransformMatrix(1, 3) = -std::get<1>(property.center);
    modelTransformMatrix(2, 2) = 1;
    modelTransformMatrix(2, 3) = -std::get<2>(property.center);
    modelTransformMatrix(3, 3) = 1;
    
    return true;
}

static int variable = 0;

void updataShaderUniformProperty(GLuint& glesProgram, TransformMatrix& modelTransformMatrix) {
    MvpTransform mvpTransform;
    mvpTransform.initModelTransformMatrix(modelTransformMatrix);
    variable++;
    Position cameraPosition;

/* square
    cameraPosition.x = 0;
    cameraPosition.y = 0;
    cameraPosition.z = -1000;
    cameraPosition.rotationX = 0;
    cameraPosition.rotationY = variable;
    cameraPosition.rotationZ = 0;
    mvpTransform.initViewTransformMatrix(cameraPosition);

    mvpTransform.initProjectionTransformMatrix(4, 1000);
*/

/* chair
    cameraPosition.x = 0;
    cameraPosition.y = 0;
    cameraPosition.z = -1000;
    cameraPosition.rotationX = -40;
    cameraPosition.rotationY = variable;
    cameraPosition.rotationZ = 0;
    mvpTransform.initViewTransformMatrix(cameraPosition);

    mvpTransform.initProjectionTransformMatrix(1.5, 1000);
*/

/*  car
    cameraPosition.x = 0;
    cameraPosition.y = -2.5;
    cameraPosition.z = -20;
    cameraPosition.rotationX = -120;
    cameraPosition.rotationY = 0;
    cameraPosition.rotationZ = variable;
    mvpTransform.initViewTransformMatrix(cameraPosition);
    mvpTransform.initProjectionTransformMatrix(2, 10);
*/

// /*  test
    std::cout << "cameraPosition.x = ";
    std::cin >> cameraPosition.x;
    std::cout << "cameraPosition.y = ";
    std::cin >> cameraPosition.y;
    std::cout << "cameraPosition.z = ";
    std::cin >> cameraPosition.z;
    std::cout << "rotationX = ";
    std::cin >> cameraPosition.rotationX;
    std::cout << "rotationY = ";
    std::cin >> cameraPosition.rotationY;
    std::cout << "rotationZ = ";
    std::cin >> cameraPosition.rotationZ;
    mvpTransform.initViewTransformMatrix(cameraPosition);

    float near, far;
    std::cout << "near = ";
    std::cin >> near;
    std::cout << "far = ";
    std::cin >> far;

    mvpTransform.initProjectionTransformMatrix(near, far);
// */

    TransformMatrix modelMatrix = mvpTransform.getModelTransformMatrix();
    auto modelMatrixLocation = glGetUniformLocation(glesProgram, "modelMatrix");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, modelMatrix.getMatrixData());

    TransformMatrix viewMatrix = mvpTransform.getViewTransformMatrix();
    auto viewMatrixLocation = glGetUniformLocation(glesProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, viewMatrix.getMatrixData());

    TransformMatrix projectionMatrix = mvpTransform.getProjectionTransformMatrix();
    auto projectionMatrixLocation = glGetUniformLocation(glesProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, projectionMatrix.getMatrixData());

    TransformMatrix normalMatrix = mvpTransform.getNormalMatrix();
    auto normalTransformLocation = glGetUniformLocation(glesProgram, "normalMatrix");
    glUniformMatrix4fv(normalTransformLocation, 1, GL_FALSE, normalMatrix.getMatrixData());

    float lightPosition[3] = {2000, 2000, 2000};
    auto lightPositionLocation = glGetUniformLocation(glesProgram, "lightPosition");
    glUniform3fv(lightPositionLocation, 1, lightPosition);

    float intensity[3] = {1.0, 1.0, 1.0};
    auto intensityLocation = glGetUniformLocation(glesProgram, "intensity");
    glUniform3fv(intensityLocation, 1, intensity);

    float Ia[3] = {0.215, 0.27, 0.385};
    auto IaLocation = glGetUniformLocation(glesProgram, "Ia");
    glUniform3fv(IaLocation, 1, Ia);

    float ka[3] = {1, 1, 1};
    auto kaLocation = glGetUniformLocation(glesProgram, "ka");
    glUniform3fv(kaLocation, 1, ka);

    float kd[3] = {0.3, 0.3, 0.3};
    auto kdLocation = glGetUniformLocation(glesProgram, "kd");
    glUniform3fv(kdLocation, 1, kd);

    float ks[3] = {0.3, 0.3, 0.3};
    auto ksLocation = glGetUniformLocation(glesProgram, "ks");
    glUniform3fv(ksLocation, 1, ks);

    float phongExp = 32;
    auto phongExpLocation = glGetUniformLocation(glesProgram, "phongExp");
    glUniform1f(phongExpLocation, phongExp);
}

int main() {
    EGLDisplay display;
    EGLSurface eglSurface;
    GLuint glesProgram;
    float* vertex;
    int vectexCount = 0;
    const char* path = "/opengles/AutoDriveDisplay/DisplaySTL/car.stl";
    TransformMatrix modelTransformMatrix;

    if (!init(display, eglSurface)) {
        std::cout << "init failed!" << std::endl;
        return 0;
    }

    if (!createGlesProgram(glesProgram)) {
        std::cout << "create gles program failed!" << std::endl;
        return 0;
    }

    if (!LoadModelVertex(path, &vertex, vectexCount, modelTransformMatrix)) {
        std::cout << "load model vertex failed!" << std::endl;
        return 0;        
    } else {
        std::cout << "vertex count = " << vectexCount << std::endl;
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        glUseProgram(glesProgram);
        glViewport (0, 0, WINDOW_WIDTH, WINDOW_HIGHT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1, 1);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        updataShaderUniformProperty(glesProgram, modelTransformMatrix);
        draw(glesProgram, vertex, vectexCount);
        eglSwapBuffers(display, eglSurface);
        std::cout << "Drawing current frame completely." << std::endl;
    }

    glDeleteProgram(glesProgram);

    delete[] vertex;

    return 0;
}