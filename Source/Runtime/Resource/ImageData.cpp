#include "ImageData.h"

namespace wind {
    uint32_t FormatToChannelNum(Format format) {
        switch(format) {
            case Format::R8G8B8A8_SINT:
            case Format::R8G8B8A8_SRGB:
            case Format::R8G8B8A8_SNORM:
            case Format::R8G8B8A8_UNORM:
                return 4;
                break;
            case Format::R8_UNORM:
                return 1;
            default:
                return 3;
        }
        return 0;
    }
}