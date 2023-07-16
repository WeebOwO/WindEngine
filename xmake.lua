set_project("WindEngine")

add_requires("glm", "glfw", "glad", "vulkansdk", "spdlog", 'assimp', 'stb', 'vulkan-memory-allocator', 'spirv-cross')

add_rules("mode.debug", "mode.release")

add_includedirs("Source/")
add_includedirs("Source/ThirdParty")

set_languages("cxx20")
set_runtimes("MD")

before_build(function (target) 
    os.exec("CompileShader.bat")
end)

after_build(function (target) 
    build_path = ""
    if is_mode("release") then
        build_path = "$(buildir)/windows/x64/release/"
    else
        build_path = "$(buildir)/windows/x64/debug/"
    end
    os.cp("Shaders/CompiledShader/**.spv", build_path)
end)

target("Sandbox")
    set_kind("binary") 
    add_files("Source/Sandbox/**.cpp")
    add_packages("spdlog")
    add_deps("Runtime")

target("Runtime")
    set_kind("static")
    add_files("Source/Runtime/**.cpp")
    add_packages("glfw", "glad", "vulkansdk", "spdlog", "assimp", "stb", "vulkan-memory-allocator", "spirv-cross", 'tinygltf')



