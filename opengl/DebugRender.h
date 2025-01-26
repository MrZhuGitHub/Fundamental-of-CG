#ifndef _DEFBUG_RENDER_H_
#define _DEFBUG_RENDER_H_

#include "shader.h"
#include "framebuffer.h"

#include <memory>

namespace CG {

class DebugRender {
public:
    DebugRender(unsigned int width, unsigned int height);
    void Render(unsigned int texture, unsigned int level);
    void SaveImage(std::string path);
    ~DebugRender();

private:
    std::shared_ptr<shader> debugShader_;
    std::shared_ptr<frameBuffer> debugFrameBuffer_;
};

}

#endif