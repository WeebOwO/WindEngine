#include "Io.h"

#include <fstream>
#include <stdexcept>

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Runtime/Base/Macro.h"

static const unsigned int ImportFlags =
    aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_SortByPType |
    aiProcess_PreTransformVertices | aiProcess_GenNormals | aiProcess_GenUVCoords |
    aiProcess_OptimizeMeshes | aiProcess_Debone | aiProcess_ValidateDataStructure;

namespace wind::io {
std::vector<char> ReadFile(std::string_view filename) {
    std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);

    if (!file.is_open()) { WIND_CORE_ERROR("Failed to open file!"); }

    size_t            fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

std::vector<uint32_t> ReadSpirvBinaryFile(std::string_view filename) {
    std::ifstream file(filename.data(), std::ios::binary);

    if (!file.is_open()) { WIND_CORE_ERROR("Failed to open file with file path {}!", filename); }

    std::vector<char> spirv((std::istreambuf_iterator<char>(file)),
                            (std::istreambuf_iterator<char>()));

    file.close();

    // Copy data from the char-vector to a new uint32_t-vector
    std::vector<uint32_t> spv(spirv.size() / sizeof(uint32_t));
    memcpy(spv.data(), spirv.data(), spirv.size());

    return spv;
}

Model::Builder LoadModelFromFilePath(std::string_view filename) {
    Model::Builder   builder;
    Assimp::Importer importer;
    auto             scene = importer.ReadFile(filename.data(), ImportFlags);
    
    if (!scene || !scene->HasMeshes()) { WIND_CORE_ERROR("Import mesh is broken!"); }
    aiMesh*             mesh = scene->mMeshes[0];
    std::vector<Vertex> vertices;

    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        vertex.normal   = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        if (mesh->HasTangentsAndBitangents()) {
            vertex.tangent   = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
            vertex.bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y,
                                mesh->mBitangents[i].z};
        }
        if (mesh->HasTextureCoords(0)) {
            vertex.texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        }
        vertices.push_back(vertex);
    }

    std::vector<uint32_t> faces;
    for (size_t i = 0; i < mesh->mNumFaces; ++i) {
        faces.push_back(mesh->mFaces[i].mIndices[0]);
        faces.push_back(mesh->mFaces[i].mIndices[1]);
        faces.push_back(mesh->mFaces[i].mIndices[2]);
    }

    builder.vertices = vertices;
    builder.indices  = faces;
    return builder;
}

} // namespace wind::io