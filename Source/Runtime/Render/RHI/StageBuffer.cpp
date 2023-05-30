#include "StageBuffer.h"

namespace wind {
StageBuffer::StageBuffer(size_t byteSize)
    : buffer(byteSize, BufferUsage::TRANSFER_SOURCE, MemoryUsage::CPU_TO_GPU), currentOffset(0) {
    (void)this->buffer.MapMemory();
}

StageBuffer::Allocation StageBuffer::Submit(const uint8_t* data, uint32_t byteSize) {
    assert(this->currentOffset + byteSize <= this->buffer.GetByteSize());

    if (data != nullptr) { this->buffer.CopyData(data, byteSize, this->currentOffset); }

    this->currentOffset += byteSize;
    return Allocation{byteSize, this->currentOffset - byteSize};
}

void StageBuffer::Reset() { this->currentOffset = 0; }

void StageBuffer::Flush() { this->buffer.FlushMemory(this->currentOffset, 0); }
} // namespace wind