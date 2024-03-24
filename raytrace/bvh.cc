#include "bvh.h"
#include "object.h"
#include "ray.h"

#include <algorithm>
#include <iostream>

namespace CG {

AabbCompare bvh::kAabbCompare = AABB_COMPARE_X;

bool aabb::hit(std::shared_ptr<ray> in) {
    double directionX = (0 != in->getDirection().xPosition ? in->getDirection().xPosition : 0.00001);
    double x1 = (xMin_ - in->getOrigin().xPosition)/directionX;
    double x2 = (xMax_ - in->getOrigin().xPosition)/directionX;
    double xMinT = (x1 < x2 ? x1 : x2);
    double xMaxT = (x1 > x2 ? x1 : x2);
    double directionY = (0 != in->getDirection().yPosition ? in->getDirection().yPosition : 0.00001);
    double y1 = (yMin_ - in->getOrigin().yPosition)/directionY;
    double y2 = (yMax_ - in->getOrigin().yPosition)/directionY;
    double yMinT = (y1 < y2 ? y1 : y2);
    double yMaxT = (y1 > y2 ? y1 : y2);
    double directionZ = (0 != in->getDirection().zPosition ? in->getDirection().zPosition : 0.00001);
    double z1 = (zMin_ - in->getOrigin().zPosition)/directionZ;
    double z2 = (zMax_ - in->getOrigin().zPosition)/directionZ;
    double zMinT = (z1 < z2 ? z1 : z2);
    double zMaxT = (z1 > z2 ? z1 : z2);

    double minMax = (xMinT < yMinT ? yMinT : xMinT);
    minMax = (minMax < zMinT ? zMinT : minMax);
    double maxMin = (xMaxT < yMaxT ? xMaxT : yMaxT);
    maxMin = (maxMin < zMaxT ? maxMin : zMaxT);

    if (minMax <= 0 && maxMin<= 0) {
        return false;
    }

    if (minMax < maxMin) {
        return true;
    } else {
        return false;
    }
}

bvh::bvh(std::shared_ptr<bvh> leftBvh, std::shared_ptr<bvh> rightBvh, std::shared_ptr<aabb> bvhAabb)
    : leftBvh_(leftBvh)
    , rightBvh_(rightBvh) {

    if (!bvhAabb) {
        if (nullptr != leftBvh && nullptr == rightBvh) {
            bvhAabb_ = leftBvh->getBvhAabb();
        } else if (nullptr == leftBvh && nullptr != rightBvh) {
            bvhAabb_ = rightBvh->getBvhAabb();
        } else {
            auto leftAabb = leftBvh->getBvhAabb();
            auto rightAabb = rightBvh->getBvhAabb();
            double minX = leftAabb->getXmin() < rightAabb->getXmin() ? leftAabb->getXmin() : rightAabb->getXmin();
            double maxX = leftAabb->getXmax() > rightAabb->getXmax() ? leftAabb->getXmax() : rightAabb->getXmax();
            double minY = leftAabb->getYmin() < rightAabb->getYmin() ? leftAabb->getYmin() : rightAabb->getYmin();
            double maxY = leftAabb->getYmax() > rightAabb->getYmax() ? leftAabb->getYmax() : rightAabb->getYmax();
            double minZ = leftAabb->getZmin() < rightAabb->getZmin() ? leftAabb->getZmin() : rightAabb->getZmin();
            double maxZ = leftAabb->getZmax() > rightAabb->getZmax() ? leftAabb->getZmax() : rightAabb->getZmax();
            bvhAabb_ = std::make_shared<aabb>(minX, maxX, minY, maxY, minZ, maxZ);
        }
    } else {
        bvhAabb_ = bvhAabb;
    }
}

std::shared_ptr<bvh> bvh::getBvhAvlTree(std::vector<std::shared_ptr<aabb>> aabbs) {
    if (aabbs.empty()) {
        return nullptr;
    } else if (1 == aabbs.size()) {
        return std::make_shared<bvh>(nullptr, nullptr, aabbs[0]);
    }
    std::vector<std::shared_ptr<aabb>> leftAabbs;
    std::vector<std::shared_ptr<aabb>> rightAabbs;
    switch ((rand()%3))
    {
        case 0:
            bvh::kAabbCompare = AABB_COMPARE_X;
            break;
        case 1:
            bvh::kAabbCompare = AABB_COMPARE_Y;
            break;
        case 2:
            bvh::kAabbCompare = AABB_COMPARE_Z;
            break;
        default:
            std::cout << "ERROR: bvh::compare is illegal!" << std::endl;
            exit(1);       
            break;
    }
    std::sort(aabbs.begin(), aabbs.end(), &bvh::compare);
    leftAabbs.insert(leftAabbs.end(), aabbs.begin(), aabbs.begin() + aabbs.size()/2);
    rightAabbs.insert(rightAabbs.end(), aabbs.begin() + aabbs.size()/2, aabbs.end());
    std::shared_ptr<bvh> leftBvh = bvh::getBvhAvlTree(leftAabbs);
    std::shared_ptr<bvh> rightBvh = bvh::getBvhAvlTree(rightAabbs);
    std::shared_ptr<bvh> root = std::make_shared<bvh>(leftBvh, rightBvh);
    return root;
}

bool bvh::compare(std::shared_ptr<aabb> aabb1, std::shared_ptr<aabb> aabb2) {
    if (AABB_COMPARE_X == bvh::kAabbCompare) {
        return (aabb1->getXmax() < aabb2->getXmax() ? true : false);
    } else if (AABB_COMPARE_Y == bvh::kAabbCompare) {
        return (aabb1->getYmax() < aabb2->getYmax() ? true : false);
    } else if (AABB_COMPARE_Z == bvh::kAabbCompare) {
        return (aabb1->getZmax() < aabb2->getZmax() ? true : false);
    } else {
        std::cout << "ERROR: bvh::compare is illegal!" << std::endl;
        exit(1);        
    }
}

bool bvh::hit(std::shared_ptr<ray> in, std::vector<std::shared_ptr<object>>& maybeHitObjects) {
    if (!bvhAabb_->hit(in)) {
        return false;
    }

    if (bvhAabb_->getObject()) {
        maybeHitObjects.push_back(bvhAabb_->getObject());
        return true;
    }

    if (leftBvh_) {
        leftBvh_->hit(in, maybeHitObjects);
    }

    if (rightBvh_) {
        rightBvh_->hit(in, maybeHitObjects);
    }

    return true;
}

}