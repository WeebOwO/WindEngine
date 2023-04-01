#pragma once

#include <string>
#include <string_view>

#include <vulkan/vulkan.hpp>

namespace wind {
class RenderPassBase {
protected:
    std::string    passName;
    vk::RenderPass renderPass;

public:
    [[nodiscard]] auto& Name() const { return passName; }
    virtual void        Setup() = 0;
    virtual void        Exec()  = 0;
};

template <typename SetupFunc, typename ExecFunc> class RenderPass : public RenderPassBase {
public:
    RenderPass(std::string_view renderPassName, const SetupFunc& setupFunc,
               const ExecFunc& execFunc)
        : m_setupFunc(setupFunc), m_exexFunc(execFunc) {
        passName = renderPassName;
    }

    std::string Name() { return passName; }

    void Setup() override { m_setupFunc(this); }

    void Exec() override { m_exexFunc(this); }

private:
    SetupFunc m_setupFunc;
    ExecFunc  m_exexFunc;
};

} // namespace wind
