# WindEngine

A personal game engine which mainly for learning purpose! Hopefully I don't end up with a graphic engine 😂.

## Build

Before you build this project, you may need to manually download [vulkansdk](https://vulkan.lunarg.com/) since xmake cannot download it automatically.

This project is using [xmake](https://xmake.io/) to manage, after download xmake, you can run commands below to build this project.

```
xmake 
xmake run
```

## Develop

If you are a developer and want to make some contributions to this project, the following environment configuration process may help you💕.

Enter the following command to add the IDE project file to the project, here is vscode as an example.

```
$ xmake project -k compile_commands
```

If you are using the [Clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) plugin, then you can find that the IDE is prompting and completing normally, and you have easily completed the configuration.

Certainly, if you are using a different IDE, you may be able to get some help [here](https://xmake.io/mirror/zh-cn/plugin/builtin_plugins.html#https://clang.llvm.org/docs/JSONCompilationDatabase.html).

## Reference

* [Game Engine Architecture 3rd Edition](https://www.gameenginebook.com/)
* [从零开始手敲次世代游戏引擎](https://zhuanlan.zhihu.com/p/510064704)
* [Hazel](https://github.com/TheCherno/Hazel)
* [Piccolo](https://github.com/BoomingTech/Piccolo)
* [SakuraEngine](https://github.com/SakuraEngine/SakuraEngine)
* [littleVulkanEngine](https://github.com/blurrypiano/littleVulkanEngine/tree/main)
