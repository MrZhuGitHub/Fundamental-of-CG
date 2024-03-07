#include "object.h"
#include "material.h"
#include "ray.h"
#include "../Common/MvpTransform.hpp"

#define SPHERE_OFFSET 0.0001
#define TRIANGLE_OFFSET 0.1

namespace CG {
object::object(std::shared_ptr<material> material)
    : material_(material) {

}

bool sphere::getSphereTextureCoordinate(const vec3& point, float& u, float& v) {
    if (abs(point.length() - 1) > 0.001) {
        return false;
    }
    double length = sqrt(point.xPosition*point.xPosition + point.zPosition*point.zPosition);
    double alpha = atan(point.yPosition/length) + (PI/2.0);
    double beta = 0;
    if (point.zPosition > 0) {
        beta = acos(point.xPosition/length);
    } else {
        beta = 2*PI - acos(point.xPosition/length);
    }
    v = alpha/(PI);
    u = beta/(2*PI);
    double offset = 0.2;
    u = u - offset;
    if (u < 0) {
        u = u + 1;
    }
    return true;
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

    float u, v;
    if (textureCoordinateValid_) {
        vec3 intersection = rayTrace->getPoint(t);
        intersection = intersection - center_;
        getTextureCoordinateFunc_(intersection.unit(), u, v);
    }

    HitFace face;
    vec3 normal(origin.xPosition + direction.xPosition*t - center_.xPosition,
                origin.yPosition + direction.yPosition*t - center_.yPosition,
                origin.zPosition + direction.zPosition*t - center_.zPosition);
    normal = normal.unit();
    if (vec3::dot(normal, direction) > 0) {
        face = HitFace::HIT_FACE_INSIDE;
        normal = -normal;
        if (!material_->scatter(rayTrace, t, normal, scatter, attenuation, face, u, v)) {
            return false;
        }
    } else {
        face = HitFace::HIT_FACE_OUTSIDE;
        if (!material_->scatter(rayTrace, t, normal, scatter, attenuation, face, u, v)) {
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

triangle::triangle(std::shared_ptr<material> material, vec3 point1, vec3 point2, vec3 point3)
    : object(material)
    , point1_(point1)
    , point2_(point2)
    , point3_(point3) {
    
    vec3 vec12 = point2_ - point1_;
    vec3 vec13 = point3_ - point1_;
    direction_ = vec3::cross(vec12, vec13);
}

bool triangle::hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation) {
    if (0 == vec3::dot(direction_, rayTrace->getDirection())) {
        return false;
    }

    double r1 = rayTrace->getDirection().xPosition * direction_.xPosition
              + rayTrace->getDirection().yPosition * direction_.yPosition
              + rayTrace->getDirection().zPosition * direction_.zPosition;
    double r2 = (point1_.xPosition - rayTrace->getOrigin().xPosition) * direction_.xPosition
              + (point1_.yPosition - rayTrace->getOrigin().yPosition) * direction_.yPosition
              + (point1_.zPosition - rayTrace->getOrigin().zPosition) * direction_.zPosition;
    t = r2/r1;
    if (t <= SPHERE_OFFSET) {
        return false;
    }

    vec3 intersection = rayTrace->getPoint(t);
    if (!triangle::getBarycentricCoordinate(point1_, point2_, point3_, intersection)) {
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

bool triangle::getBarycentricCoordinate(const vec3& triangleP1, const vec3& triangleP2, const vec3& triangleP3, vec3& result) {
    double i = (-(result.xPosition - triangleP2.xPosition)*(triangleP3.yPosition - triangleP2.yPosition) +
                (result.yPosition - triangleP2.yPosition)*(triangleP3.xPosition - triangleP2.xPosition))/
               (-(triangleP1.xPosition - triangleP2.xPosition)*(triangleP3.yPosition - triangleP2.yPosition) +
                (triangleP1.yPosition - triangleP2.yPosition)*(triangleP3.xPosition - triangleP2.xPosition));
    double j = (-(result.xPosition - triangleP3.xPosition)*(triangleP1.yPosition - triangleP3.yPosition) + 
                (result.yPosition - triangleP3.yPosition)*(triangleP1.xPosition - triangleP3.xPosition))/
               (-(triangleP2.xPosition - triangleP3.xPosition)*(triangleP1.yPosition - triangleP3.yPosition) +
                (triangleP2.yPosition - triangleP3.yPosition)*(triangleP1.xPosition - triangleP3.xPosition));
    double k = 1 - i - j;
    if (i < 0 || j < 0 || k < 0) {
        return false;
    } else {
        return true;
    }
}

std::shared_ptr<aabb> triangle::getAabb() {
    double xMin, xMax, yMin, yMax, zMin, zMax;

    xMin = point1_.xPosition < point2_.xPosition ? point1_.xPosition : point2_.xPosition;
    xMin = xMin < point3_.xPosition ? xMin : point3_.xPosition;
    xMax = point1_.xPosition > point2_.xPosition ? point1_.xPosition : point2_.xPosition;
    xMax = xMax > point3_.xPosition ? xMax : point3_.xPosition;

    yMin = point1_.yPosition < point2_.yPosition ? point1_.yPosition : point2_.yPosition;
    yMin = yMin < point3_.yPosition ? yMin : point3_.yPosition;
    yMax = point1_.yPosition > point2_.yPosition ? point1_.yPosition : point2_.yPosition;
    yMax = yMax > point3_.yPosition ? yMax : point3_.yPosition;

    zMin = point1_.zPosition < point2_.zPosition ? point1_.zPosition : point2_.zPosition;
    zMin = zMin < point3_.zPosition ? zMin : point3_.zPosition;
    zMax = point1_.zPosition > point2_.zPosition ? point1_.zPosition : point2_.zPosition;
    zMax = zMax > point3_.zPosition ? zMax : point3_.zPosition;

    if (xMin == xMax) {
        xMin = xMin - TRIANGLE_OFFSET;
        xMax = xMax + TRIANGLE_OFFSET;
    }

    if (yMin == yMax) {
        yMin = yMin - TRIANGLE_OFFSET;
        yMax = yMax + TRIANGLE_OFFSET;
    }

    if (zMin == zMax) {
        zMin = zMin - TRIANGLE_OFFSET;
        zMax = zMax + TRIANGLE_OFFSET;
    }

    return std::make_shared<aabb>(xMin, xMax, yMin, yMax, zMin, zMax, shared_from_this());
}

parallelogram::parallelogram(std::shared_ptr<material> material, vec3 point1, vec3 point2, vec3 point3)
    : object(material)
    , point1_(point1)
    , point2_(point2)
    , point3_(point3) {
      baseVec1_ = point1 - point2;
      baseVec2_ = point3 - point2;
      point4_ = baseVec1_ + baseVec2_ + point2;
      direction_ = vec3::cross(baseVec1_, baseVec2_);  
}

bool parallelogram::hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation) {
    if (0 == vec3::dot(direction_, rayTrace->getDirection())) {
        return false;
    }

    double r1 = rayTrace->getDirection().xPosition * direction_.xPosition
              + rayTrace->getDirection().yPosition * direction_.yPosition
              + rayTrace->getDirection().zPosition * direction_.zPosition;
    double r2 = (point1_.xPosition - rayTrace->getOrigin().xPosition) * direction_.xPosition
              + (point1_.yPosition - rayTrace->getOrigin().yPosition) * direction_.yPosition
              + (point1_.zPosition - rayTrace->getOrigin().zPosition) * direction_.zPosition;
    t = r2/r1;
    if (t <= SPHERE_OFFSET) {
        return false;
    }

    vec3 intersection = rayTrace->getPoint(t);
    if ((!triangle::getBarycentricCoordinate(point1_, point2_, point3_, intersection))
        && (!triangle::getBarycentricCoordinate(point1_, point4_, point3_, intersection))) {
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

std::shared_ptr<aabb> parallelogram::getAabb() {
    double xMin, xMax, yMin, yMax, zMin, zMax;

    xMin = point1_.xPosition < point2_.xPosition ? point1_.xPosition : point2_.xPosition;
    xMin = xMin < point3_.xPosition ? xMin : point3_.xPosition;
    xMin = xMin < point4_.xPosition ? xMin : point4_.xPosition;
    xMax = point1_.xPosition > point2_.xPosition ? point1_.xPosition : point2_.xPosition;
    xMax = xMax > point3_.xPosition ? xMax : point3_.xPosition;
    xMax = xMax > point4_.xPosition ? xMax : point4_.xPosition;

    yMin = point1_.yPosition < point2_.yPosition ? point1_.yPosition : point2_.yPosition;
    yMin = yMin < point3_.yPosition ? yMin : point3_.yPosition;
    yMin = yMin < point4_.yPosition ? yMin : point4_.yPosition;
    yMax = point1_.yPosition > point2_.yPosition ? point1_.yPosition : point2_.yPosition;
    yMax = yMax > point3_.yPosition ? yMax : point3_.yPosition;
    yMax = yMax > point4_.yPosition ? yMax : point4_.yPosition;

    zMin = point1_.zPosition < point2_.zPosition ? point1_.zPosition : point2_.zPosition;
    zMin = zMin < point3_.zPosition ? zMin : point3_.zPosition;
    zMin = zMin < point4_.zPosition ? zMin : point4_.zPosition;
    zMax = point1_.zPosition > point2_.zPosition ? point1_.zPosition : point2_.zPosition;
    zMax = zMax > point3_.zPosition ? zMax : point3_.zPosition;
    zMax = zMax > point4_.zPosition ? zMax : point4_.zPosition;

    if (xMin == xMax) {
        xMin = xMin - TRIANGLE_OFFSET;
        xMax = xMax + TRIANGLE_OFFSET;
    }

    if (yMin == yMax) {
        yMin = yMin - TRIANGLE_OFFSET;
        yMax = yMax + TRIANGLE_OFFSET;
    }

    if (zMin == zMax) {
        zMin = zMin - TRIANGLE_OFFSET;
        zMax = zMax + TRIANGLE_OFFSET;
    }

    return std::make_shared<aabb>(xMin, xMax, yMin, yMax, zMin, zMax, shared_from_this());
}

vec3 parallelogram::getPoint(float u, float v) {
    vec3 point = u * baseVec1_ + v * baseVec2_ + point2_;
    return point;
}

}