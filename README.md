[![License](https://img.shields.io/badge/licence-MIT-green.svg?style=flat)](LICENSE)
[![ThorVGPT](https://img.shields.io/badge/ThorVGPT-76A99C?style=flat&logo=openai&logoColor=white)](https://chat.openai.com/g/g-Ht3dYIwLO-thorvgpt)
[![Discord](https://img.shields.io/badge/Community-5865f2?style=flat&logo=discord&logoColor=white)](https://discord.gg/n25xj6J6HM)
[![OpenCollective](https://img.shields.io/badge/OpenCollective-84B5FC?style=flat&logo=opencollective&logoColor=white)](https://opencollective.com/thorvg)
<br>
[![Build](https://github.com/thorvg/thorvg.example/actions/workflows/build_test.yml/badge.svg?branch=main&event=push)](https://github.com/thorvg/thorvg.example/actions/workflows/build_test.yml)


# ThorVG Example
<p align="center">
  <img width="800" height="auto" src="https://github.com/thorvg/thorvg.site/blob/main/readme/logo/512/thorvg-banner.png">
</p>

ThorVG Example provides a wide range of example codes. Those examples demonstrate how to use ThorVG's APIs to render vector graphics, handle images, and play animations. Each example is designed to highlight specific features and help developers get started quickly with ThorVG. <br />
 
## How To
This section details the steps required to configure the environment for installing ThorVG Example. Please note that [ThorVG](https://github.com/thorvg/thorvg) and [SDL](https://www.libsdl.org/) must be installed on your system before building the examples.
<br />
### Build
ThorVG Example supports [Meson](https://mesonbuild.com/) build system. Install [meson](http://mesonbuild.com/Getting-meson.html) and [ninja](https://ninja-build.org/) if you don't have them already.

Run Meson to configure ThorVG Example in the project root folder.
```
meson setup builddir
```
Run ninja to build
```
ninja -C builddir
```
### Run
Once the build is complete, you can run each example binary directly from the builddir folder.

To run an example with the GL or WGPU engine, pass 'gl' or 'wg' as a command-line argument. By default, the examples run with the software rendering engine.
```
./builddir/src/Shapes gl    # Runs with OpenGL/ES
./builddir/src/Shapes wg    # Runs with WebGPU
./builddir/src/Shapes       # Runs with Software (default)
```

<img width="1997" height="auto" alt="image" src="https://github.com/user-attachments/assets/90df1659-2ad1-4a81-89c9-33f6cc7a467e" />
