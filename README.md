# MDCII

Another unofficial 1602 AD project. However, the focus here is not on reading the original files, but on creating an editable
isometric game world.

You must have a copy of the original game installed on your computer.

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/50ffb066523c4064ab513bef2f8d4628)](https://www.codacy.com/gh/stwe/MDCII/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=stwe/MDCII&amp;utm_campaign=Badge_Grade)

<img src="https://github.com/stwe/MDCII/blob/main/resources/devlog/01-01-2023.png" width="500" height="400" alt="" />

* [Compiling MDCII](#compiling-mdcii)
* [Running MDCII](#running-mdcii)
* [Devlog](#devlog)
* [Roadmap](#roadmap-mdcii-v02)
* [Reporting bugs](#reporting-bugs)
* [License](#license)

## Compiling MDCII

### Requirements

* [GLFW](https://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [GLM](https://github.com/g-truc/glm)
* [spdlog](https://github.com/gabime/spdlog)
* [Magic Enum C++](https://github.com/Neargye/magic_enum)
* [FreeType](https://freetype.org/)
* [Protocol Buffers](https://github.com/protocolbuffers/protobuf)
* C++ 17 Compiler (Gcc or MSVC)

### Platforms

* [Win with Visual Studio and Premake](#win-with-visual-studio-2022-and-premake)
* [Win with MinGW64 and Cmake](#win-with-mingw64-and-cmake)
* [Linux and Cmake](#linux-with-cmake)

#### Win with Visual Studio 2022 and Premake

The process is switched to Conan 2.0.

#### Win with MinGW64 and Cmake

The process is switched to Conan 2.0.

#### Linux with Cmake

The process is switched to Conan 2.0.

## Running MDCII

The original game is needed.

For a program start so-called tile atlas graphics are needed, which have to be created first. For this purpose a
`TileAtlasCreator` is located in the `install` directory. See [README](https://github.com/stwe/MDCII/blob/main/install/TileAtlasCreator/README.md).

In the MDCII `config.ini` the paths must be adapted.

**The first start takes longer, because the `haeuser.cod` will be parsed.**

## Devlog

**28.01.2023** Add a tag for v0.1.

## Roadmap MDCII v0.2

* refactoring menus and add missing translations
* World Generator2 is renamed to IslandGenerator; the files created in this way becomes a new file extension.
* a new editor combines the islands created with the IslandGenerator into a map/world
* it should be possible to build roads; another Layer may be created for this
* animations should be possible showing how residents brings goods from A to B; for this the A* algorithm can be implemented
* further down my priority list: the terrain is to be expanded to include rivers and mountains

## Reporting bugs

You can use GitHub [issues](https://github.com/stwe/MDCII/issues) for reporting bugs.

## License

MDCII is licensed under the GPL-2.0 License, see [LICENSE](https://github.com/stwe/MDCII/blob/main/LICENSE) for more information.
