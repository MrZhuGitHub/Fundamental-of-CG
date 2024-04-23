#include "shader.h"

#include "glm/gtc/type_ptr.hpp"

namespace CG {

shader::shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();       

        vShaderFile.close();
        fShaderFile.close();

        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();     
    } catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void shader::use() {
    glUseProgram(ID);
}

void shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
}

void shader::setViewMatrix(glm::mat4 viewMatrix) {
    glUniformMatrix4fv(glGetUniformLocation(ID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

void shader::setProjectionMatrix(glm::mat4 projectionMatrix) {
    glUniformMatrix4fv(glGetUniformLocation(ID, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

void shader::setModelMatrix(glm::mat4 modelMatrix) {
    glUniformMatrix4fv(glGetUniformLocation(ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void shader::setLight() {
    float lightPosition[3] = {2000, 2000, 2000};
    auto lightPositionLocation = glGetUniformLocation(ID, "lightPosition");
    glUniform3fv(lightPositionLocation, 1, lightPosition);

    float intensity[3] = {1.0, 1.0, 1.0};
    auto intensityLocation = glGetUniformLocation(ID, "intensity");
    glUniform3fv(intensityLocation, 1, intensity);

    float Ia[3] = {0.215, 0.27, 0.385};
    auto IaLocation = glGetUniformLocation(ID, "Ia");
    glUniform3fv(IaLocation, 1, Ia);

    float ka[3] = {1, 1, 1};
    auto kaLocation = glGetUniformLocation(ID, "ka");
    glUniform3fv(kaLocation, 1, ka);

    float kd[3] = {0.3, 0.3, 0.3};
    auto kdLocation = glGetUniformLocation(ID, "kd");
    glUniform3fv(kdLocation, 1, kd);

    float ks[3] = {0.3, 0.3, 0.3};
    auto ksLocation = glGetUniformLocation(ID, "ks");
    glUniform3fv(ksLocation, 1, ks);

    float phongExp = 32;
    auto phongExpLocation = glGetUniformLocation(ID, "phongExp");
    glUniform1f(phongExpLocation, phongExp);
}

shader::~shader() {
    glDeleteProgram(ID);
}

}