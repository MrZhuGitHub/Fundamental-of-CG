#include "sample.h"

namespace CG {

HemisphereCosSample::HemisphereCosSample(vec3 point, vec3 normal)
    : point_(point)
    , normal_(normal.unit())
    , onb_(point, normal.unit()) {

}

vec3 HemisphereCosSample::generateRandomSample(vec3 startPoint) {
    double r1 = (double)(rand()%10000)/10000.0;
    double r2 = (double)(rand()%10000)/10000.0;
    double z = sqrt(1 - r2);
    double x = cos(2*PI*r1)*sqrt(r2);
    double y = sin(2*PI*r1)*sqrt(r2);
    return onb_.local(x, y, z);
}

double HemisphereCosSample::getRandomSamplePdf(vec3 startPoint, vec3 endPoint) {
    vec3 direction = endPoint - startPoint;
    double result = vec3::dot(direction.unit(), normal_.unit())/PI;
    if (result < 0) {
        return 0;
    } else {
        return result;
    }
}

}