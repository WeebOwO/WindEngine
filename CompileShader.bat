@REM glslc.exe Shaders/ForwardBasePass.vert -o Shaders/CompiledShader/ForwardBasePass.vert.spv
@REM glslc.exe Shaders/ForwardBasePass.frag -o Shaders/CompiledShader/ForwardBasePass.frag.spv
@REM glslc.exe Shaders/SkyBox.vert -o Shaders/CompiledShader/SkyBox.vert.spv
@REM glslc.exe Shaders/SkyBox.frag -o Shaders/CompiledShader/SkyBox.frag.spv

python Scripts/ShaderCompile.py