#ifndef _RENDER_PIPELINE_H_
#define _RENDER_PIPELINE_H_

#include "model"

namespace CG {

class renderPipeline {
public:
    void LoadModel(std::shared_ptr<model> model, );
    void RenderDirectLighting();
    void RenderIndirectLighting();
    void RenderAmbientOcclusion();
    void RenderShadow();

private:

};


}

#endif