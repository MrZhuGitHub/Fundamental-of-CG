#ifndef _BRDF_H_
#define _BRDF_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace CG {

class BRDF {
public:
    virtual glm::vec3 getBrdf(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal) = 0; 

private:
}

}

#endif