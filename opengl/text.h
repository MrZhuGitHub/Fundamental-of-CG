#ifndef _TEXT_H_
#define _TEXT_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

#include <map>
#include <string>
#include <memory>

namespace CG {

struct character
{
    unsigned int textId;
    glm::ivec2 size;
    glm::ivec2 bearing;
    long int advance;
};

class text {
public:
    text(std::shared_ptr<shader> textShader, float width, float height);
    ~text();

    bool loadCharacters(std::string ttf);
    void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

private:
    std::map<char, character> characters_;
    unsigned int VAO_;
    unsigned int VBO_;
    std::shared_ptr<shader> textShader_;
};

}

#endif