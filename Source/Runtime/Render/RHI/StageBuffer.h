#pragma once

#include "Runtime/Render/RHI/Buffer.h"

namespace wind {
class StageBuffer {
    Buffer   buffer;
    uint32_t currentOffset;

public:
    struct Allocation {
        uint32_t Size;
        uint32_t Offset;
    };

    StageBuffer(size_t byteSize);

    Allocation    Submit(const uint8_t* data, uint32_t byteSize);
    void          Flush();
    void          Reset();
    Buffer&       GetBuffer() { return this->buffer; }
    const Buffer& GetBuffer() const { return this->buffer; }
    uint32_t      GetCurrentOffset() const { return this->currentOffset; }

    template <typename T> Allocation Submit(std::span<const T> view) {
        return this->Submit((const uint8_t*)view.data(), uint32_t(view.size() * sizeof(T)));
    }

    template <typename T> Allocation Submit(std::span<T> view) {
        return this->Submit((const uint8_t*)view.data(), uint32_t(view.size() * sizeof(T)));
    }

    template <typename T> Allocation Submit(const T* value) {
        return this->Submit((uint8_t*)value, uint32_t(sizeof(T)));
    }
};
} // namespace wind