#include "Mesh.h"

#include "Runtime/Base/Utils.h"
#include "Runtime/Render/RHI/Backend.h"

#include <memory>

namespace wind {
Model::Model(Builder builder)
    : m_vertexCnt(builder.vertices.size()), m_indexCnt(builder.indices.size()) {
    vk::DeviceSize vertexBufferSize = sizeof(builder.vertices[0]) * m_vertexCnt;
    vk::DeviceSize indexBufferSize  = sizeof(builder.indices[0]) * m_indexCnt;

    auto& backend     = RenderBackend::GetInstance();
    auto& stageBuffer = backend.GetStagingBuffer();
    auto  cmdBuffer   = backend.BeginSingleTimeCommand();

    m_vertexBuffer = std::make_shared<Buffer>(
        vertexBufferSize, BufferUsage::VERTEX_BUFFER | BufferUsage::TRANSFER_DESTINATION,
        MemoryUsage::GPU_ONLY);
    m_indexBuffer = std::make_shared<Buffer>(
        indexBufferSize, BufferUsage::INDEX_BUFFER | BufferUsage::TRANSFER_DESTINATION,
        MemoryUsage::GPU_ONLY);
    
    const auto vertexAllocation = stageBuffer.Submit(utils::MakeView(builder.vertices));
    const auto indexAllocation  = stageBuffer.Submit(utils::MakeView(builder.indices));

    cmdBuffer.CopyBuffer(BufferInfo{stageBuffer.GetBuffer(), vertexAllocation.Offset},
                         BufferInfo{*m_vertexBuffer, 0}, vertexAllocation.Size);
    cmdBuffer.CopyBuffer(BufferInfo{stageBuffer.GetBuffer(), indexAllocation.Offset},
                         BufferInfo{*m_indexBuffer, 0}, indexAllocation.Size);

    stageBuffer.Flush();
    backend.SubmitSingleTimeCommand(cmdBuffer.GetNativeHandle());

    stageBuffer.Reset();
}

Model::~Model() {
    m_indexBuffer.reset();
    m_vertexBuffer.reset();
}

void Model::Bind(CommandBuffer& cmdbuffer) {
    cmdbuffer.BindVertexBuffers(*m_vertexBuffer);
    cmdbuffer.BindIndexBufferUInt32(*m_indexBuffer);
}

void Model::Draw(CommandBuffer& cmdbuffer) { 
    cmdbuffer.DrawIndexed(m_indexCnt, 1); 
}
} // namespace wind