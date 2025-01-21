#ifndef _HIERACHICAL_DEPTH_RENDER_H_
#define _HIERACHICAL_DEPTH_RENDER_H_

namespace CG {

class HierachicalDepthRender {
public:
    HierachicalDepthRender();
    ~HierachicalDepthRender();
    void render();

private:
    unsigned int VAO_;
    unsigned int VBO_;
};

}

#endif