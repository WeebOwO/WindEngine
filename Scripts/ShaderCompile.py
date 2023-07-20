import os
import subprocess

def Judge(x):
    return x.endswith("vert") or x.endswith("frag") or x.endswith("comp")

def CompileShader():
    # use this black list to check no need shader
    black_list = []

    if not os.path.exists("Shaders/CompiledShader/"):
        os.mkdir("Shaders/CompiledShader/")

    current_dir = os.getcwd()
    
    shader_root_path = os.path.join(current_dir, "Shaders")
    shader_file_list = [file for file in os.listdir(shader_root_path) if Judge(file)]

    for shader in shader_file_list:
        if shader in black_list:
            continue
        shader_path = os.path.join(shader_root_path, shader)
        compile_out_path = os.path.join(shader_root_path, "CompiledShader", shader + ".spv")
        subprocess.Popen(["glslc.exe", shader_path, "-o", compile_out_path])
    return

if __name__ == "__main__":
    CompileShader()