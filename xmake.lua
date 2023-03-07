set_project("Vulkan Engine")

add_requires("glm", "glfw", "glad", "vulkansdk")

add_rules("mode.debug", "mode.release")
add_includedirs("source/")
set_languages("cxx20")

after_build(function (target) 
    build_path = ""
    if is_mode("release") then
        build_path = "$(buildir)/windows/x64/release/"
    else
        build_path = "$(buildir)/windows/x64/debug/"
    end
    os.cp("shaders/compiled_shader/**.spv", build_path)
end)

target("runtime")
    set_kind("static")
    add_files("source/runtime/**.cpp")
    add_packages("glfw", "glad", "vulkansdk")
    
target("sandbox")
    set_kind("binary") 
    add_files("source/sandbox/**.cpp")
    add_packages("glfw", "glad", "vulkansdk")
    add_deps("runtime")


