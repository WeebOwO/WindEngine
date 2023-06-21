#pragma once

#include <memory>

#include "Runtime/Scene/GameObject.h"
#include "Runtime/Scene/Scene.h"

namespace wind {

struct ViewInfo {
    ViewInfo();
    struct CameraBuffer {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;
    };

    struct ObjectBuffer {
        glm::mat4 model;
    };

    void Init();

    void UpdateCameraBuffer(Camera* camera);
    void UpdateObjectBuffer(GameObject* gameObject); 
    
    std::shared_ptr<Buffer> GetBuffer(const std::string& bufferName);
    
private:
    std::unordered_map<std::string, std::shared_ptr<Buffer>> predefinedBuffer;
    std::shared_ptr<Buffer> m_camearaUniformBuffer;
    std::shared_ptr<Buffer> m_objectUniformBuffer;
};

// A scene abstraction for renderer side data
class SceneView {
public:
    SceneView() = default;
    SceneView(Scene* scene);
    void  SetScene(Scene* scene);
    auto* GetOwnScene() { return m_scene; }
    auto& GetViewInfo() { return m_viewInfo; }

private:
    Scene*   m_scene;
    ViewInfo m_viewInfo;
};
} // namespace wind