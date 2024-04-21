#ifndef _SHADER_H_
#define _SHADER_H_

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>

namespace CG {

class shader {
public:
    shader(const char* vertexPath, const char* fragmentPath);
    ~shader();

    void use();

    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;

    void setViewProjectionMatrix(glm::mat4 viewProjectionMatrix);
    void setModelMatrix(glm::mat4 modelMatrix);

private:
    unsigned int ID;
};

}

#endif