#include "Scene.h"

namespace wind {
    void Scene::Init() {
        WIND_CORE_INFO("Scene is Init");
    }

    Scene::Scene() {
        size_t cameraViewSize = sizeof(CameraView);
        m_uniformBuffers.resize(UniformBufferGroup::UniformBufferCnt, nullptr);
        m_uniformBuffers[UniformBufferGroup::viewUniform] = std::make_shared<Buffer>(cameraViewSize, BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);
    }

    void Scene::UpdateUniformBuffer() {
        auto& viewUniformBuffer = m_uniformBuffers[UniformBufferGroup::viewUniform];
        // SetupviewUniformBuffer
        CameraView view = {m_activeCamera->GetView(), m_activeCamera->GetProjection()};
        viewUniformBuffer->MapMemory();
        viewUniformBuffer->CopyData(reinterpret_cast<uint8_t*>(&view), sizeof(CameraView), 0);
        viewUniformBuffer->UnmapMemory();
    }
}