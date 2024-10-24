// #include "screenSpaceReflections.h"

// namespace CG {

// ScreenSpaceReflections::ScreenSpaceReflections(std::shared_ptr<Environment> environmentLight)
//     : environmentLight_(environmentLight) {

//     MicrofacetBRDF::getPreComputerResult(preComputerEavg_, preComputerBrdf_);
//     environmentLight_ = environmentLight_->getPreComputerResult();
// }

// ScreenSpaceReflections::~ScreenSpaceReflections() {

// }

// void ScreenSpaceReflections::CreateShadingProgram() {
//     if (!CreateShadowMapProgram()) {
//         return;
//     }

//     if (!CreateDirectShadingProgram()) {
//         return;
//     }

//     if (!CreateIndirectShadingProgram()) {
//         return;
//     }
// }

// void ScreenSpaceReflections::Render() {
//     if (!GenerateShadowMap()) {
//         return;
//     }

//     if (!CreateDirectShadingProgram()) {

//     }

//     if (!CreateIndirectShadingProgram()) {
        
//     }
// }

// bool ScreenSpaceReflections::CreateShadowMapProgram() {

// }

// bool ScreenSpaceReflections::GenerateShadowMap() {

// }

// bool ScreenSpaceReflections::CreateDirectShadingProgram() {

// }

// bool ScreenSpaceReflections::ShadingDirectLgiht() {

// }

// bool ScreenSpaceReflections::CreateIndirectShadingProgram() {

// }

// bool ScreenSpaceReflections::ShadingIndirectLight() {

// }

// }