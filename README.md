# WindEngine
Currently just a graphic engine based on vulkan, may extend to a game engine in the future.

## Build
This project is using [xmake](https://xmake.io/) to manage, after download xmake, you can run commands below to build this project.

```
xmake 
xmake run
```

## Feature
* RenderGraph 
* Shader reflection for auto create pipeline layout
* Forward / Deferred Shading (Tiled Based Implementation)
  
### Todo
- [ ] PBR + IBL
- [ ] Shadow
- [ ] Skinned Animation
- [ ] Post Process pipeline
- [ ] GPU Driven (Draw Indirect, CS Culling..) 

## Reference
*  [Hazel](https://github.com/TheCherno/Hazel)
*  [Piccolo](https://github.com/BoomingTech/Piccolo)
*  [SakuraEngine](https://github.com/SakuraEngine/SakuraEngine)
*  [littleVulkanEngine](https://github.com/blurrypiano/littleVulkanEngine/tree/main)
