#include "camera.h"
#include "object.h"
#include "ray.h"
#include "bvh.h"

namespace CG {

camera::camera(double height, double width, uint32_t pixelWidth, uint32_t pixelHeight,
    double len, Position position)
    : height_(height),
      width_(width),
      pixelWidth_(pixelWidth),
      pixelHeight_(pixelHeight),
      len_(len),
      position_(position),
      ppmHead_(false) {
    TransformMatrix translationMatrix;
    translationMatrix(0, 0) = 1;
    translationMatrix(1, 1) = 1;
    translationMatrix(2, 2) = 1;
    translationMatrix(3, 3) = 1;
    translationMatrix(0, 3) = position_.x;
    translationMatrix(1, 3) = position_.y;
    translationMatrix(2, 3) = position_.z;
    TransformMatrix XrotationMatrix;
    XrotationMatrix(0, 0) = 1;
    XrotationMatrix(1, 1) = cos(position_.rotationX*PI/180);
    XrotationMatrix(1, 2) = -sin(position_.rotationX*PI/180);
    XrotationMatrix(2, 1) = sin(position_.rotationX*PI/180);
    XrotationMatrix(2, 2) = cos(position_.rotationX*PI/180);
    XrotationMatrix(3, 3) = 1;
    TransformMatrix YrotationMatrix;
    YrotationMatrix(1, 1) = 1;
    YrotationMatrix(0, 0) = cos(position_.rotationY*PI/180);
    YrotationMatrix(0, 2) = sin(position_.rotationY*PI/180);
    YrotationMatrix(2, 0) = -sin(position_.rotationY*PI/180);
    YrotationMatrix(2, 2) = cos(position_.rotationY*PI/180);
    YrotationMatrix(3, 3) = 1;
    TransformMatrix ZrotationMatrix;
    ZrotationMatrix(2, 2) = 1;
    ZrotationMatrix(0, 0) = cos(position_.rotationZ*PI/180);
    ZrotationMatrix(0, 1) = -sin(position_.rotationZ*PI/180);
    ZrotationMatrix(1, 0) = sin(position_.rotationZ*PI/180);
    ZrotationMatrix(1, 1) = cos(position_.rotationZ*PI/180);
    ZrotationMatrix(3, 3) = 1;
    transformMatrix_ = translationMatrix*XrotationMatrix*YrotationMatrix*ZrotationMatrix;
}

void camera::render(std::vector<std::shared_ptr<object>> objects, uint32_t launchCount, uint32_t samples) {

    std::vector<std::shared_ptr<aabb>> aabbs;
    for (auto& object : objects) {
        aabbs.push_back(object->getAabb());
    }
    std::shared_ptr<bvh> rootBvh = bvh::getBvhAvlTree(aabbs);

    double* imageBuffer = (double*)malloc(pixelHeight_*pixelWidth_*3*sizeof(double));
    memset((void*)(imageBuffer), 0 ,pixelHeight_*pixelWidth_*3*sizeof(double));
    for (uint32_t yIndex = 0; yIndex < pixelHeight_; yIndex++) {
        for (uint32_t xIndex = 0; xIndex < pixelWidth_; xIndex++) {
            auto renderPixelTask = [&](uint32_t x, uint32_t y, uint32_t samples)->void {
                auto rays = this->getRays(x, y, samples);
                color pixelColor(0, 0, 0);
                for (auto& ray : rays) {
                    color sampleColor = ray->getRayColor(rootBvh, launchCount);
                    pixelColor = pixelColor + sampleColor;
                }
                pixelColor = pixelColor/samples;
                imageBuffer[(y*pixelWidth_ + x)*3] = pixelColor.xPosition;
                imageBuffer[(y*pixelWidth_ + x)*3 + 1] = pixelColor.yPosition;
                imageBuffer[(y*pixelWidth_ + x)*3 + 2] = pixelColor.zPosition;
            };
            threadpool_.push(std::bind(renderPixelTask, xIndex, yIndex, samples));
        }
    }

    while (threadpool_.waitTask() != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (int pixelIndex = 0; pixelIndex < (pixelWidth_*pixelHeight_); pixelIndex++) {
        double r = imageBuffer[pixelIndex*3];
        double g = imageBuffer[pixelIndex*3 + 1];
        double b = imageBuffer[pixelIndex*3 + 2];
        color pixel(r, g, b);
        writeColor((pixelIndex%pixelWidth_), (pixelIndex/pixelWidth_), pixel);
    }
    free(imageBuffer);
}

void camera::writeColor(unsigned int pixelWidth, unsigned int pixelHeight, color pixelColor) {
    if (!ppmHead_) {
        std::cout << "P3\n" << pixelWidth_ << ' ' << pixelHeight_ << "\n255\n";
        ppmHead_ = true;
    }
    if (pixelColor.xPosition > 1) {
        pixelColor.xPosition = 1;
    }
    if (pixelColor.yPosition > 1) {
        pixelColor.yPosition = 1;
    }
    if (pixelColor.zPosition > 1) {
        pixelColor.zPosition = 1;
    }
    int ir = static_cast<int>(255.999 * pixelColor.xPosition);
    int ig = static_cast<int>(255.999 * pixelColor.yPosition);
    int ib = static_cast<int>(255.999 * pixelColor.zPosition);
    std::cout << ir << ' ' << ig << ' ' << ib << '\n';
}

std::vector<std::shared_ptr<ray>> camera::getRays(const uint32_t& xPixel, const uint32_t& yPixel, uint32_t count) {
    std::vector<std::shared_ptr<ray>> rays;
    for (int i = 0; i < count; i++) {
        double x = (width_/pixelWidth_)*(xPixel + (rand()%10000)/10000.0);
        double y = (height_/pixelHeight_)*(yPixel + (rand()%10000)/10000.0);
        x = x - width_/2;
        y = y - height_/2;
        vec3 origin = cameraTransform(vec3(0, 0, 0));
        vec3 direction = cameraTransform(vec3(x, y, len_)) - origin;
        auto sampleRay = std::make_shared<ray>(origin, direction);
        rays.push_back(sampleRay);
    }
    return rays;
}

vec3 camera::cameraTransform(vec3 sample) {
    Vertex vec(sample.xPosition, sample.yPosition, sample.zPosition);
    Vertex result = transformMatrix_*vec;
    return vec3(result.xPosition, result.yPosition, result.zPosition);
}

}