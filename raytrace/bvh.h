#ifndef _BVH_H_
#define _BVH_H_

#include <memory>
#include <vector>

#include "ray.h"

namespace CG {

enum AabbCompare {
    AABB_COMPARE_X = 0,
    AABB_COMPARE_Y,
    AABB_COMPARE_Z,
};

class object;

class aabb {
public:
    aabb(double xMin, double xMax, double yMin,
        double yMax, double zMin, double zMax,
        std::shared_ptr<object> object = nullptr)
    : xMin_(xMin)
    , xMax_(xMax)
    , yMin_(yMin)
    , yMax_(yMax)
    , zMin_(zMin)
    , zMax_(zMax)
    , object_(object) {

    }

    aabb(aabb& object) {
        xMin_ = object.getXmin();
        xMax_ = object.getXmax();
        yMin_ = object.getYmin();
        yMax_ = object.getYmax();
        zMin_ = object.getZmin();
        zMax_ = object.getZmax();
        object_ = object.getObject();
    }

    aabb& operator=(aabb& object) {
        xMin_ = object.getXmin();
        xMax_ = object.getXmax();
        yMin_ = object.getYmin();
        yMax_ = object.getYmax();
        zMin_ = object.getZmin();
        zMax_ = object.getZmax();
        return (*this);
    }

    aabb& operator=(aabb&& object) {
        xMin_ = object.getXmin();
        xMax_ = object.getXmax();
        yMin_ = object.getYmin();
        yMax_ = object.getYmax();
        zMin_ = object.getZmin();
        zMax_ = object.getZmax();
        return (*this);
    }

    bool hit(std::shared_ptr<ray> in);

    std::shared_ptr<object> getObject() const { return object_; };

    double getXmin() const { return xMin_; };

    double getXmax() const { return xMax_; };

    double getYmin() const { return yMin_; };

    double getYmax() const { return yMax_; };

    double getZmin() const { return zMin_; };

    double getZmax() const { return zMax_; };

private:
    double xMin_;
    double xMax_;
    double yMin_;
    double yMax_;
    double zMin_;
    double zMax_;
    std::shared_ptr<object> object_;
};

class bvh {
public:
    bvh(std::shared_ptr<bvh> leftBvh, std::shared_ptr<bvh> rightBvh, std::shared_ptr<aabb> bvhAabb = nullptr);

    static std::shared_ptr<bvh> getBvhAvlTree(std::vector<std::shared_ptr<aabb>> aabbs);

    static bool compare(std::shared_ptr<aabb> aabb1, std::shared_ptr<aabb> aabb2);

    bool hit(std::shared_ptr<ray> in, std::vector<std::shared_ptr<object>>& maybeHitObjects);

    std::shared_ptr<aabb> getBvhAabb() const { return bvhAabb_;};

private:
    std::shared_ptr<aabb> bvhAabb_;
    std::shared_ptr<bvh> leftBvh_;
    std::shared_ptr<bvh> rightBvh_;
    static AabbCompare kAabbCompare;
};
}

#endif