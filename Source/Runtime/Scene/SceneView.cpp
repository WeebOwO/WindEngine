#include "SceneView.h"
#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Buffer.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {

ViewInfo::ViewInfo() { Init(); }

void ViewInfo::Init() {
    // create camera uniform buffer
    size_t cameraBufferSize = sizeof(CameraBuffer);
    m_camearaUniformBuffer = std::make_shared<Buffer>(cameraBufferSize, BufferUsage::UNIFORM_BUFFER,
                                                      MemoryUsage::CPU_TO_GPU);
    predefinedBuffer["CameraBuffer"] = m_camearaUniformBuffer;
    // create object uniform buffer
    size_t objectBufferSize = sizeof(ObjectBuffer);
    m_objectUniformBuffer = std::make_shared<Buffer>(objectBufferSize, BufferUsage::UNIFORM_BUFFER,
                                                     MemoryUsage::CPU_TO_GPU);
    predefinedBuffer["ObjectBuffer"] = m_objectUniformBuffer;
}

void ViewInfo::UpdateCameraBuffer(Camera* camera) {
    CameraBuffer cameraBuffer{camera->GetView(), camera->GetProjection(),
                              camera->GetView() * camera->GetProjection()};
    m_camearaUniformBuffer->MapMemory();
    m_camearaUniformBuffer->CopyData((uint8_t*)&cameraBuffer, sizeof(cameraBuffer), 0);
    m_camearaUniformBuffer->UnmapMemory();
}

std::shared_ptr<Buffer> ViewInfo::GetBuffer(const std::string& bufferName) {
    if(predefinedBuffer.find(bufferName) == predefinedBuffer.end()) {
        WIND_CORE_WARN("Failed to find {}", bufferName);
    }
    return predefinedBuffer[bufferName];
}

void SceneView::SetScene(Scene* scene) { 
    m_scene = scene; 
    m_viewInfo.UpdateCameraBuffer(scene->GetActiveCamera().get());
}

SceneView::SceneView(Scene* scene) : m_scene(scene) {
    m_viewInfo.Init();
}
} // namespace wind