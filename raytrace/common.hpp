#ifndef _COMMON_H_
#define _COMMON_H_

#include "math.h"

namespace CG
{
class vec3 {
public:
    vec3(double x = 0, double y = 0, double z = 0)
        : xPosition(x)
        , yPosition(y)
        , zPosition(z)
    {

    }

    vec3(vec3& v) {
        xPosition = v.xPosition;
        yPosition = v.yPosition;
        zPosition = v.zPosition;
    }

    vec3(vec3&& v) {
        xPosition = v.xPosition;
        yPosition = v.yPosition;
        zPosition = v.zPosition;
    }

    vec3& operator=(vec3& v) {
        xPosition = v.xPosition;
        yPosition = v.yPosition;
        zPosition = v.zPosition;
        return (*this);
    }

    vec3& operator=(vec3&& v) {
        xPosition = v.xPosition;
        yPosition = v.yPosition;
        zPosition = v.zPosition;
        return (*this);
    }

    vec3 operator+(vec3&& v) {
        vec3 result;
        result.xPosition = xPosition + v.xPosition;
        result.yPosition = yPosition + v.yPosition;
        result.zPosition = zPosition + v.zPosition;
        return result;
    }

    vec3 operator+(vec3& v) {
        vec3 result;
        result.xPosition = xPosition + v.xPosition;
        result.yPosition = yPosition + v.yPosition;
        result.zPosition = zPosition + v.zPosition;
        return result;
    }

    vec3 operator-(vec3& v) {
        vec3 result;
        result.xPosition = xPosition - v.xPosition;
        result.yPosition = yPosition - v.yPosition;
        result.zPosition = zPosition - v.zPosition;
        return result;
    }

    vec3 operator-(vec3&& v) {
        vec3 result;
        result.xPosition = xPosition - v.xPosition;
        result.yPosition = yPosition - v.yPosition;
        result.zPosition = zPosition - v.zPosition;
        return result;
    }

    vec3 operator*(double multiplier) const {
        vec3 result;
        result.xPosition = xPosition * multiplier;
        result.yPosition = yPosition * multiplier;
        result.zPosition = zPosition * multiplier;
        return result;
    }

    vec3 operator*(vec3&& v) {
        vec3 result;
        result.xPosition = v.xPosition * xPosition;
        result.yPosition = v.yPosition * yPosition;
        result.zPosition = v.zPosition * zPosition;
        return result;
    }

    vec3 operator/(double divisor) {
        vec3 result;
        result.xPosition = xPosition / divisor;
        result.yPosition = yPosition / divisor;
        result.zPosition = zPosition / divisor;
        return result;
    }

    vec3 operator-() const {
        vec3 result;
        result.xPosition = -xPosition;
        result.yPosition = -yPosition;
        result.zPosition = -zPosition;
        return result;
    }

    long double length() const {
        return sqrt(xPosition*xPosition + yPosition*yPosition + zPosition*zPosition);
    }

    vec3 unit() const {
        return vec3(xPosition/length(), yPosition/length(), zPosition/length());
    }

    static double dot(const vec3& v1, const vec3& v2) {
        return (v1.xPosition*v2.xPosition + v1.yPosition*v2.yPosition + v1.zPosition*v2.zPosition);
    }

    static vec3 cross(const vec3& v1, const vec3& v2) {
        return vec3(v1.yPosition * v2.zPosition - v1.zPosition * v2.yPosition,
                    v1.zPosition * v2.xPosition - v1.xPosition * v2.zPosition,
                    v1.xPosition * v2.yPosition - v1.yPosition * v2.xPosition);
    }

    static vec3 random(double min, double max) {
        double range = max - min;
        return vec3((double)(rand()%10000)*range/10000.0 + min,
                    (double)(rand()%10000)*range/10000.0 + min,
                    (double)(rand()%10000)*range/10000.0 + min);
    }

public:
    double xPosition;
    double yPosition;
    double zPosition;
};

using color = vec3; 

inline vec3 operator*(const double& multiplier, const vec3& v1) {
    vec3 result;
    result.xPosition = v1.xPosition * multiplier;
    result.yPosition = v1.yPosition * multiplier;
    result.zPosition = v1.zPosition * multiplier;
    return result;
}

} // namespace CG


#endif