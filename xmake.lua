add_requires("glm", "glfw", "glad", "vulkansdk")

add_rules("mode.debug", "mode.release")
add_includedirs("source/")
add_includedirs("source/3rd/")
set_languages("cxx20")

target("runtime")
    set_kind("static")
    add_files("source/runtime/**.cpp")
    add_packages("glfw", "glad", "vulkansdk")
    
target("engine")
    set_kind("binary") 
    add_files("source/*.cpp")
    add_packages("glfw", "glad", "vulkansdk")
    add_deps("runtime")