#include "ImageLoader.h"

#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYDDSLOADER_IMPLEMENTATION
#include <ThirdParty/tinyddsloader.h>

namespace wind {
static bool IsDDSImage(const std::string& filepath) {
    std::filesystem::path filename{filepath};
    return filename.extension() == ".dds";
}

static bool IsZLIBImage(const std::string& filepath) {
    std::filesystem::path filename{filepath};
    return filename.extension() == ".zlib";
}

static Format DDSFormatToImageFormat(tinyddsloader::DDSFile::DXGIFormat format) {
    switch (format) {
    case tinyddsloader::DDSFile::DXGIFormat::Unknown:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_Float:
        return Format::R32G32B32A32_SFLOAT;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_UInt:
        return Format::R32G32B32A32_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_SInt:
        return Format::R32G32B32A32_SINT;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_Float:
        return Format::R32G32B32_SFLOAT;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_UInt:
        return Format::R32G32B32_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_SInt:
        return Format::R32G32B32_SINT;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_Float:
        return Format::R16G16B16A16_SFLOAT;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UNorm:
        return Format::R16G16B16A16_UNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UInt:
        return Format::R16G16B16A16_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SNorm:
        return Format::R16G16B16A16_SNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SInt:
        return Format::R16G16B16A16_SINT;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32_Float:
        return Format::R32G32_SFLOAT;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32_UInt:
        return Format::R32G32_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R32G32_SInt:
        return Format::R32G32_SINT;
    case tinyddsloader::DDSFile::DXGIFormat::R32G8X24_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::D32_Float_S8X24_UInt:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R32_Float_X8X24_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::X32_Typeless_G8X24_UInt:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UNorm:
        return Format::A2R10G10B10_UNORM_PACK_32;
    case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UInt:
        return Format::A2R10G10B10_UINT_PACK_32;
    case tinyddsloader::DDSFile::DXGIFormat::R11G11B10_Float:
        return Format::B10G11R11_UFLOAT_PACK_32;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm:
        return Format::R8G8B8A8_UNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm_SRGB:
        return Format::R8G8B8A8_SRGB;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UInt:
        return Format::R8G8B8A8_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SNorm:
        return Format::R8G8B8A8_SNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SInt:
        return Format::R8G8B8A8_SINT;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16_Float:
        return Format::R16G16_SFLOAT;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16_UNorm:
        return Format::R16G16_UNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16_UInt:
        return Format::R16G16_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16_SNorm:
        return Format::R16G16_SNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R16G16_SInt:
        return Format::R16G16_SINT;
    case tinyddsloader::DDSFile::DXGIFormat::R32_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::D32_Float:
        return Format::D32_SFLOAT;
    case tinyddsloader::DDSFile::DXGIFormat::R32_Float:
        return Format::R32_SFLOAT;
    case tinyddsloader::DDSFile::DXGIFormat::R32_UInt:
        return Format::R32_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R32_SInt:
        return Format::R32_SINT;
    case tinyddsloader::DDSFile::DXGIFormat::R24G8_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::D24_UNorm_S8_UInt:
        return Format::D24_UNORM_S8_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R24_UNorm_X8_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::X24_Typeless_G8_UInt:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8_UNorm:
        return Format::R8G8_UNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8_UInt:
        return Format::R8G8_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8_SNorm:
        return Format::R8G8_SNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8_SInt:
        return Format::R8G8_SINT;
    case tinyddsloader::DDSFile::DXGIFormat::R16_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R16_Float:
        return Format::R16_SFLOAT;
    case tinyddsloader::DDSFile::DXGIFormat::D16_UNorm:
        return Format::D16_UNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R16_UNorm:
        return Format::R16_UNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R16_UInt:
        return Format::R16_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R16_SNorm:
        return Format::R16_SNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R16_SInt:
        return Format::R16_SINT;
    case tinyddsloader::DDSFile::DXGIFormat::R8_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R8_UNorm:
        return Format::R8_UNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R8_UInt:
        return Format::R8_UINT;
    case tinyddsloader::DDSFile::DXGIFormat::R8_SNorm:
        return Format::R8_SNORM;
    case tinyddsloader::DDSFile::DXGIFormat::R8_SInt:
        return Format::R8_SINT;
    case tinyddsloader::DDSFile::DXGIFormat::A8_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R1_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R9G9B9E5_SHAREDEXP:
        return Format::E5B9G9R9_UFLOAT_PACK_32;
    case tinyddsloader::DDSFile::DXGIFormat::R8G8_B8G8_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::G8R8_G8B8_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC1_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm_SRGB:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC2_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm_SRGB:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC3_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm_SRGB:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC4_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC4_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC4_SNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC5_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC5_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC5_SNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::B5G6R5_UNorm:
        return Format::B5G6R5_UNORM_PACK_16;
    case tinyddsloader::DDSFile::DXGIFormat::B5G5R5A1_UNorm:
        return Format::B5G5R5A1_UNORM_PACK_16;
    case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm:
        return Format::B8G8R8A8_UNORM;
    case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::R10G10B10_XR_BIAS_A2_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm_SRGB:
        return Format::B8G8R8A8_SRGB;
    case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_UNorm_SRGB:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC6H_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC6H_UF16:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC6H_SF16:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC7_Typeless:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm_SRGB:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::AYUV:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::Y410:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::Y416:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::NV12:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::P010:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::P016:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::YUV420_OPAQUE:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::YUY2:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::Y210:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::Y216:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::NV11:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::AI44:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::IA44:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::P8:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::A8P8:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::B4G4R4A4_UNorm:
        return Format::B4G4R4A4_UNORM_PACK_16;
    case tinyddsloader::DDSFile::DXGIFormat::P208:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::V208:
        return Format::UNDEFINED;
    case tinyddsloader::DDSFile::DXGIFormat::V408:
        return Format::UNDEFINED;
    default:
        return Format::UNDEFINED;
    }
}

ImageData ImageLoader::LoadImageFromFile(const std::string& filepath) {

}

CubemapData ImageLoader::LoadCubemapImageFromFile(const std::string& filepath) {

}
} // namespace wind