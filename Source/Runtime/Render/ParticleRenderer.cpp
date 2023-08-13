#include "ParticleRenderer.h"

namespace wind {
    void ParticlePassRendering() {
        
    }

    void ParticleRenderer::Render(Scene& scene) {
        m_backend.StartFrame();
        
        m_backend.EndFrame();
    }

    void ParticleRenderer::Init() {

    }

    void ParticleRenderer::InitView(Scene& scene) {
        m_sceneView->SetScene(&scene);
    };
}