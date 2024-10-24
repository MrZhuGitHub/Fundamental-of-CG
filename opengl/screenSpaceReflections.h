// #ifndef _GI_SSR_H_
// #define _GI_SSR_H_

// #include "environmentLight.h"
// #include "microfacetBrdf.h"

// namespace CG {

// class ScreenSpaceReflections {
// public:
//     ScreenSpaceReflections(std::shared_ptr<EnvironmentLight> environmentLight);
//     ~ScreenSpaceReflections();

//     void CreateShadingProgram();
//     void Render();

// private:
//     bool CreateShadowMapProgram();
//     bool GenerateShadowMap();

//     bool CreateDirectShadingProgram();
//     bool ShadingDirectLgiht();

//     bool CreateIndirectShadingProgram();
//     bool ShadingIndirectLight();

// private:
//     unsigned int preComputerBrdf_; //BRDF1micro BRDF2micro
//     unsigned int preComputerEavg_;
//     unsigned int environmentLightTexture_; //IBL
//     std::shared_ptr<EnvironmentLight> environmentLight_;

//     unsigned int shadowMapFrameBuffer_;
//     unsigned int shadowMapProgram_;

//     unsigned int directShadingFrameBuffer_;
//     unsigned int directShadingProgram_;

//     unsigned int indirectShadingFrameBuffer_;
//     unsigned int indirectShadingProgram_;
// }

// }

// #endif