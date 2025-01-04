#ifndef _FRAME_BUFFER_H_
#define _FRAME_BUFFER_H_

namespace CG {

enum PIXEL{
    R = 0,
    G,
    B,
    A,
};

class frameBuffer {
public:
    frameBuffer(unsigned int width, unsigned int height, bool mass = false, unsigned int samples = 4);

    ~frameBuffer();

    bool init();

    bool setup();

    bool unload();

    bool blitToFrameBuffer(unsigned int frameBuffer);

    unsigned int getTexture() const {
        return textureId_;
    }

    unsigned int getFrameBuffer() const {
        return frameBufferId_;
    }

    unsigned int getRenderBuffer() const {
        return renderBufferId_;
    }

    bool readPixels(unsigned int x, unsigned int y, float* pixels);

    bool readPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height, float* pixels);

private:
    unsigned int textureId_;
    unsigned int frameBufferId_;
    unsigned int renderBufferId_;
    unsigned int width_;
    unsigned int height_;
    bool mass_;
    unsigned int samples_;
    bool initSuccess_;
};

}

#endif