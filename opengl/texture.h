#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "environmentLight.h"

#include <map>

namespace CG {

class textureLoader {
public:
    textureLoader();

    ~textureLoader();

    bool set2Dtexture(std::string image, bool mipmap);

    bool set3Dtexture(std::map<CubeTextureId, std::string> images, bool mipmap);

    unsigned int getTextureId();

private:
    unsigned int textureId_;
    bool textureIfValid_;
};

}

#endif