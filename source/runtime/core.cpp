#include "core.h"

#include "GLFW/glfw3.h"

#include <chrono>
#include <memory>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "runtime/base/macro.h"
#include "runtime/base/utils.h"

#include "runtime/render/context.h"
#include "runtime/render/renderer.h"
#include "runtime/render/window.h"
#include "runtime/resource/model.h"
#include "runtime/resource/scene.h"
#include "runtime/resource/vertex.h"

static constexpr uint32_t WIDTH  = 1920;
static constexpr uint32_t HEIGHT = 1080;

namespace wind {
// EngienImpl part
class EngineImpl {
public:
    EngineImpl() : m_window(WIDTH, HEIGHT, "Vulkan Engine") {
        Log::Init();
        RenderContext::Init(m_window.GetWindow());
        Scene::Init();
        m_renderer = std::make_unique<Renderer>(m_window);

    }

    ~EngineImpl() = default;

    void Run() {
        LoadGameObjects();
        while (!glfwWindowShouldClose(m_window.GetWindow())) {
            LogicTick();
            RenderTick();
        }
    }

private:
    void                      RenderTick();
    void                      LogicTick();
    void                      LoadGameObjects() const noexcept;
    
    Window                    m_window;
    std::unique_ptr<Renderer> m_renderer;
};

void EngineImpl::LogicTick() { glfwPollEvents(); }

void EngineImpl::RenderTick() { m_renderer->DrawFrame(); }

void EngineImpl::LoadGameObjects() const noexcept {
    Model::Builder            builder;
    const unsigned int ImportFlags = 
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_PreTransformVertices |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_OptimizeMeshes |
		aiProcess_Debone |
		aiProcess_ValidateDataStructure;

    const std::string skyboxFile = R"(D:\Dev\WindEngine\assets\meshes\skybox.obj)";
    Assimp::Importer importer;
    auto scene = importer.ReadFile(skyboxFile, ImportFlags);

    if(!scene || !scene->HasMeshes()) {
        WIND_CORE_ERROR("Import mesh is broken!");
    }
    aiMesh* mesh = scene->mMeshes[0];

    std::vector<Vertex> vertices;
    for(size_t i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
		vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
		vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
		if(mesh->HasTangentsAndBitangents()) {
			vertex.tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
			vertex.bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
		}
		if(mesh->HasTextureCoords(0)) {
			vertex.texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
		}
		vertices.push_back(vertex);
    }
    
    builder.vertices = vertices;

    std::vector<uint32_t> faces;
    for(size_t i = 0; i < mesh->mNumFaces; ++i) {
        faces.push_back(mesh->mFaces[i].mIndices[0]);
        faces.push_back(mesh->mFaces[i].mIndices[1]);
        faces.push_back(mesh->mFaces[i].mIndices[2]);
    }
    builder.indices = faces;
    auto& world = Scene::GetWorld();
    world.AddModel(builder);
}

// Engine Part
void Engine::Run() { m_impl->Run(); }

Engine::Engine() : m_impl(std::make_unique<EngineImpl>()) {}

Engine::~Engine() {
    auto& device = utils::GetRHIDevice();
    device.waitIdle();
}

} // namespace wind