# MDCII

Another unofficial 1602 AD project. However, the focus here is not on reading the original files, but on creating an editable
isometric game world.

You must have a copy of the original game installed on your computer.

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/50ffb066523c4064ab513bef2f8d4628)](https://www.codacy.com/gh/stwe/MDCII/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=stwe/MDCII&amp;utm_campaign=Badge_Grade)

* [Compiling MDCII](#compiling-mdcii)
* [Running MDCII](#running-mdcii)
* [Roadmap](#roadmap-mdcii-v02)
* [Reporting bugs](#reporting-bugs)
* [License](#license)

## Compiling MDCII

### Using Vcpkg with CMake

### Using Vcpkg with CMake and Visual Studio Code

### Requirements

* [spdlog](https://github.com/gabime/spdlog)
* [imgui](https://github.com/ocornut/imgui)
* [Protocol Buffers](https://github.com/protocolbuffers/protobuf)
* C++ 20 Compiler (Gcc or MSVC)

### Platforms

The project runs on Windows and Linux.

## Running MDCII

The original game is needed.

For a program start so-called tile atlas graphics are needed, which have to be created first. For this purpose a
`TileAtlasCreator` is located in the `install` directory. See [README](https://github.com/stwe/MDCII/blob/main/install/TileAtlasCreator/README.md).

In the MDCII `config.ini` the paths must be adapted.

**The first start takes longer, because the `haeuser.cod` will be parsed.**

## Roadmap MDCII v0.2

* sound system: play background music and sound effects
* refactoring menus and add missing translations
* it should be possible to build roads
* animations should be possible showing how residents brings goods from A to B; for this the A* algorithm can be implemented
* further down my priority list: the terrain is to be expanded to include rivers and mountains

## Reporting bugs

You can use GitHub [issues](https://github.com/stwe/MDCII/issues) for reporting bugs.

## License

MDCII is licensed under the GPL-2.0 License, see [LICENSE](https://github.com/stwe/MDCII/blob/main/LICENSE) for more information.
