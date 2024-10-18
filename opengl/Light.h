#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace CG {

class Light {
public:
virtual glm::vec3 getRadiance(glm::vec3 direction) = 0;

}

}

#endif
