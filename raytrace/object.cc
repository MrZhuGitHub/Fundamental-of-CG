#include "object.h"
#include "material.h"
#include "ray.h"

#define SPHERE_OFFSET 0.0001

namespace CG {
object::object(std::shared_ptr<material> material)
    : material_(material) {
}

sphere::sphere(std::shared_ptr<material> material, vec3 center, double radius)
    : object(material)
    , center_(center)
    , radius_(radius)
{

}

bool sphere::hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation)
{
    vec3 origin = rayTrace->getOrigin();
    vec3 direction = rayTrace->getDirection();
    double c2 = pow(direction.xPosition, 2) + pow(direction.yPosition, 2) + pow(direction.zPosition, 2);
    double c1 = 2*direction.xPosition*(origin.xPosition - center_.xPosition)
                + 2*direction.yPosition*(origin.yPosition - center_.yPosition)
                + 2*direction.zPosition*(origin.zPosition - center_.zPosition);
    double c0 = pow((origin.xPosition - center_.xPosition), 2)
                + pow((origin.yPosition - center_.yPosition), 2)
                + pow((origin.zPosition - center_.zPosition), 2)
                - pow(radius_, 2);
    double delta = c1*c1 - 4*c0*c2;
    if (delta < 0) {
        return false;
    }
    double x1 = (-c1 + sqrt(c1*c1 - 4*c0*c2))/(2*c2);
    double x2 = (-c1 - sqrt(c1*c1 - 4*c0*c2))/(2*c2);

    if (abs(x1) < SPHERE_OFFSET) {
        x1 = 0;
    }

    if (abs(x2) < SPHERE_OFFSET) {
        x2 = 0;
    }

    if (x1 > 0 && x2 > 0) {
        t = x2 > x1 ? x1 : x2;
    } else if (x1 <= 0 && x2 > 0) {
        t = x2;
    } else if (x1 > 0 && x2 <= 0) {
        t = x1;
    } else {
        return false;
    }
    HitFace face;
    vec3 normal(origin.xPosition + direction.xPosition*t - center_.xPosition,
                origin.yPosition + direction.yPosition*t - center_.yPosition,
                origin.zPosition + direction.zPosition*t - center_.zPosition);
    normal = normal.unit();
    if (vec3::dot(normal, direction) > 0) {
        face = HitFace::HIT_FACE_INSIDE;
        normal = -normal;
        if (!material_->scatter(rayTrace, t, normal, scatter, attenuation, face)) {
            return false;
        }
    } else {
        face = HitFace::HIT_FACE_OUTSIDE;
        if (!material_->scatter(rayTrace, t, normal, scatter, attenuation, face)) {
            return false;
        }
    }
    return true;
}

std::shared_ptr<aabb> sphere::getAabb()
{
    return std::make_shared<aabb>(center_.xPosition - radius_, center_.xPosition + radius_,
                                  center_.yPosition - radius_, center_.yPosition + radius_,
                                  center_.zPosition - radius_, center_.zPosition + radius_,
                                  shared_from_this());
}

ground::ground(std::shared_ptr<material> material, vec3 point, vec3 direction) 
    : object(material)
    , point_(point)
    , direction_(direction.unit()) {

}

bool ground::hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation) {
    if (0 == vec3::dot(direction_, rayTrace->getDirection())) {
        return false;
    }

    double r1 = rayTrace->getDirection().xPosition * direction_.xPosition
              + rayTrace->getDirection().yPosition * direction_.yPosition
              + rayTrace->getDirection().zPosition * direction_.zPosition;
    double r2 = (point_.xPosition - rayTrace->getOrigin().xPosition) * direction_.xPosition
              + (point_.yPosition - rayTrace->getOrigin().yPosition) * direction_.yPosition
              + (point_.zPosition - rayTrace->getOrigin().zPosition) * direction_.zPosition;
    t = r2/r1;
    if (t <= SPHERE_OFFSET) {
        return false;
    }
    if (vec3::dot(rayTrace->getDirection(), direction_) > 0) {
        if (!material_->scatter(rayTrace, t, -direction_, scatter, attenuation, HitFace::HIT_FACE_INSIDE)) {
            return false;
        }
    } else {
        if (!material_->scatter(rayTrace, t, direction_, scatter, attenuation, HitFace::HIT_FACE_OUTSIDE)) {
            return false;
        }
    }
    return true;
}

std::shared_ptr<aabb> ground::getAabb()
{
    return std::make_shared<aabb>((double)(0xffffffffffffffff), (double)(0xffffffffffffffff>>1),
                                  (double)(0xffffffffffffffff), (double)(0xffffffffffffffff>>1),
                                  (double)(0xffffffffffffffff), (double)(0xffffffffffffffff>>1),
                                  shared_from_this());
}

}