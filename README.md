# WindEngine
Currently just a graphic engine based on vulkan, may extend to a game engine in the future.

## Build
This project is using [xmake](https://xmake.io/) to manage, after download xmake, you can run commands below to build this project.

```shell
xmake 
xmake run
```

If you want to generate vs project file, you can also run commands below.

```shell
xmake project -k vsxmake -m "debug,release"
```

## Feature
* RenderGraph 
* Shader Reflection For Auto Create Render Pipeline
* Forward / Defer Shading Pipeline
* Orbit / FirstPerson Camera 
* PBR + IBL 
* Shadow Map with PCF Filter
* Tangent Space Normal Mapping
* ACES ToneMapping
* HDR Pipeline
* GLTF Support with TinyGLTF
* DrawIndirect and ComputeShader 
  
## Screenshot

<table>
    <td><center><img src = "Assets/Picture/PBR.png" alt = "PBR + IBL"></center></td>
    <td><center><img src = "Assets/Picture/DeferShading.png"alt = "DeferShading"></center></td>
</table>

## Reference
*  [VulkanAbstractionLayer](https://github.com/asc-community/VulkanAbstractionLayer)
*  [Hazel](https://github.com/TheCherno/Hazel)
*  [Piccolo](https://github.com/BoomingTech/Piccolo)
*  [SakuraEngine](https://github.com/SakuraEngine/SakuraEngine)
*  [littleVulkanEngine](https://github.com/blurrypiano/littleVulkanEngine/tree/main)
*  [PBR](https://github.com/Nadrin/PBR)
