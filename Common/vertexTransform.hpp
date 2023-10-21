#ifndef _VERTEX_TRANSFORM_H_
#define _VERTEX_TRANSFORM_H_

#include "loadStlMode.hpp"

namespace VertexTransform {

inline bool translation(const std::vector<Point> pointsIn, std::vector<Point>& pointsOut, double x, double y, double z) {
    for (auto point : pointsIn) {
        double xPoint = std::get<0>(point);
        double yPoint = std::get<1>(point);
        double zPoint = std::get<2>(point);
        xPoint = xPoint + x;
        yPoint = yPoint + y;
        zPoint = zPoint + z;
        pointsOut.push_back(Point(xPoint, yPoint, zPoint));
    }
    return true;
}

inline bool scale(const std::vector<Point> pointsIn, std::vector<Point>& pointsOut, double xRatio, double yRatio, double zRatio, Point center) {
    for (auto point : pointsIn) {
        double xPoint = std::get<0>(point);
        double yPoint = std::get<1>(point);
        double zPoint = std::get<2>(point);
        xPoint = std::get<0>(center) + (xPoint - std::get<0>(center))*xRatio;
        yPoint = std::get<1>(center) + (yPoint - std::get<1>(center))*yRatio;
        zPoint = std::get<2>(center) + (zPoint - std::get<2>(center))*zRatio;
        pointsOut.push_back(Point(xPoint, yPoint, zPoint));
    }
    return true;
}

inline bool rotateX() {
    return false;
}

inline bool rotateY() {
    return false;
}

inline bool rotateZ() {
    return false;
}
}


#endif