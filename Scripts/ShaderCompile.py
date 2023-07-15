import os
import subprocess

def CompileShader():
    black_list = ["Triangle.vert", "Triangle.frag"]

    if not os.path.exists("Shaders/CompiledShader/"):
        os.mkdir("Shaders/CompiledShader/")

    current_dir = os.getcwd()
    
    shader_root_path = os.path.join(current_dir, "Shaders")
    shader_file_list = [file for file in os.listdir(shader_root_path) if file.endswith("vert") or file.endswith("frag")]

    for shader in shader_file_list:
        if shader in black_list:
            continue
        shader_path = os.path.join(shader_root_path, shader)
        compile_out_path = os.path.join(shader_root_path, "CompiledShader", shader + ".spv")
        subprocess.Popen(["glslc.exe", shader_path, "-o", compile_out_path])
    return

if __name__ == "__main__":
    CompileShader()