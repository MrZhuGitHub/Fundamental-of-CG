#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string>
#include <unordered_map>

#include "common.hpp"

namespace CG {

class texture {
public:
    bool loadTexture(std::string filePath);

    color lookup(float u, float v);

private:
    std::unordered_map<unsigned int, color> colorMaps_;
    int imageWidth_;
    int imageHeight_;
};

}

#endif