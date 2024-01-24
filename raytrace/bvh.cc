#include "bvh.h"
#include "object.h"
#include "ray.h"

#include <algorithm>

namespace CG {

bool aabb::hit(std::shared_ptr<ray> in) {

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
            double minX = leftAabb->getXmin() : rightAabb->getXmin() ? leftAabb->getXmin() < rightAabb->getXmin();
            double maxX = leftAabb->getXmax() : rightAabb->getXmax() ? leftAabb->getXmax() > rightAabb->getXmax();
            double minY = leftAabb->getYmin() : rightAabb->getYmin() ? leftAabb->getYmin() < rightAabb->getYmin();
            double maxY = leftAabb->getYmax() : rightAabb->getYmax() ? leftAabb->getYmax() > rightAabb->getYmax();
            double minZ = leftAabb->getZmin() : rightAabb->getZmin() ? leftAabb->getZmin() < rightAabb->getZmin();
            double maxZ = leftAabb->getZmax() : rightAabb->getZmax() ? leftAabb->getZmax() > rightAabb->getZmax();
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
    std::sort(aabbs.begin(), aabbs.end(), &bvh::compare);
    leftAabbs.insert(leftAabbs.end(), aabbs.begin(), aabbs.begin() + aabbs.size()/2);
    rightAabbs.insert(rightAabbs.end(), aabbs.begin() + aabbs.size()/2, aabbs.end());
    std::shared_ptr<bvh> leftBvh = bvh::getBvhAvlTree(leftAabbs);
    std::shared_ptr<bvh> rightBvh = bvh::getBvhAvlTree(rightAabbs);
    std::shared_ptr<bvh> root = std::make_shared<bvh>(leftBvh, rightBvh);
    return root;
}

bool bvh::compare(std::shared_ptr<aabb>& aabb1, std::shared_ptr<aabb>& aabb2) {

}

bool bvh::hit(std::shared_ptr<ray> in, double hitRecord, std::vector<std::shared_ptr<aabb>> leafAabbs) {

}

}