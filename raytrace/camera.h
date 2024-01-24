#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <vector>
#include <memory>
#include <iostream>

#include "../Common/MvpTransform.hpp"
#include "../Common/loadStlMode.hpp"
#include "common.hpp"
#include "threadpool.hpp"

namespace CG {

class object;
class ray;

class camera {
public:
    camera(double height, double width, uint32_t pixelWidth, uint32_t pixelHeight, double len, Position position);

    void render(std::vector<std::shared_ptr<object>> objects, uint32_t launchCount, uint32_t samples);

private:
    void writeColor(unsigned int pixelWidth, unsigned int pixelHeight, color pixelColor);

    std::vector<std::shared_ptr<ray>> getRays(const uint32_t& xPixel, const uint32_t& yPixel, uint32_t count);

    vec3 cameraTransform(vec3 sample);

private:
    double height_;
    double width_;
    uint32_t pixelWidth_;
    uint32_t pixelHeight_;
    double len_;
    Position position_;
    TransformMatrix transformMatrix_;
    bool ppmHead_;
    Utility::threadpool threadpool_;
};

}

#endif