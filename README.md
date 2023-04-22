# MDCII

Another unofficial 1602 AD project. However, the focus here is not on reading the original files, but on creating an editable
isometric game world.

You must have a copy of the original game installed on your computer.

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/50ffb066523c4064ab513bef2f8d4628)](https://www.codacy.com/gh/stwe/MDCII/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=stwe/MDCII&amp;utm_campaign=Badge_Grade)

<img src="https://github.com/stwe/MDCII/blob/main/resources/devlog/First_MapEditor_15-04-2023.png" width="500" height="400" alt="" />

<img src="https://github.com/stwe/MDCII/blob/main/resources/devlog/15-04-2023.png" width="500" height="400" alt="" />

* [Compiling MDCII](#compiling-mdcii)
* [Running MDCII](#running-mdcii)
* [Roadmap](#roadmap-mdcii-v02)
* [Reporting bugs](#reporting-bugs)
* [License](#license)

## Compiling MDCII

I prefer vcpkg. You can install it as described [here.](https://vcpkg.io/en/getting-started.html)

Now the [Requirements](#requirements) have to be installed.

### Requirements

* [GLFW](https://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [GLM](https://github.com/g-truc/glm)
* [spdlog](https://github.com/gabime/spdlog)
* [imgui](https://github.com/ocornut/imgui)
* [FreeType](https://freetype.org/)
* [Protocol Buffers](https://github.com/protocolbuffers/protobuf)
* [openal-soft](https://github.com/kcat/openal-soft)
* [libsndfile](https://github.com/libsndfile/libsndfile)
* C++ 17 Compiler (Gcc or MSVC)

### Platforms

The project runs on Windows and Linux.

## Running MDCII

The original game is needed.

For a program start so-called tile atlas graphics are needed, which have to be created first. For this purpose a
`TileAtlasCreator` is located in the `install` directory. See [README](https://github.com/stwe/MDCII/blob/main/install/TileAtlasCreator/README.md).

In the MDCII `config.ini` the paths must be adapted.

**The first start takes longer, because the `haeuser.cod` will be parsed.**

## Roadmap MDCII v0.2

The project now contains a simple Perlin Noise based Island Generator. The island files created with this
can be loaded with the World Editor and placed anywhere in the world. A new game can be started with the
map files created with the World Editor.

* sound system: play background music and sound effects
* refactoring menus and add missing translations
* it should be possible to build roads; another Layer may be created for this
* animations should be possible showing how residents brings goods from A to B; for this the A* algorithm can be implemented
* further down my priority list: the terrain is to be expanded to include rivers and mountains

## Reporting bugs

You can use GitHub [issues](https://github.com/stwe/MDCII/issues) for reporting bugs.

## License

MDCII is licensed under the GPL-2.0 License, see [LICENSE](https://github.com/stwe/MDCII/blob/main/LICENSE) for more information.
