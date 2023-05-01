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

### Using Vcpkg with CMake

You can use any package manager. Currently, I'm using [Vcpkg](https://vcpkg.io/en/). I also deleted all that extra `Premake5` stuff.

I can't explain every IDE here, but the short `Vcpkg` story is as follows:
- Clone/install and bootstrap [Vcpkg](https://github.com/Microsoft/vcpkg#getting-started) - Those who use the `CLion`
  IDE can now use their newest Vcpkg plugin.
- Install the packages mentioned below.
- Link the Vcpkg toolchain file.

### Using Vcpkg with CMake and Visual Studio Code

To install `Vcpkg`, you can follow the official instructions on [GitHub](https://github.com/Microsoft/vcpkg#getting-started).

**Step 1: Clone `Vcpkg`**

```bash
git clone https://github.com/Microsoft/vcpkg.git
```

**Step 2: Run the bootstrap script to build `Vcpkg`**

Make sure you are in the `Vcpkg` directory.
```bash
./bootstrap-vcpkg.sh
```

**Step 3: Install libraries**

Make sure you are in the `Vcpkg` directory and install the [Requirements](#requirements):
```bash
./vcpkg install glfw3 glew glm spdlog imgui freetype protobuf openal-soft libsndfile
```

**Step 4: Linking the `Vcpkg` toolchain file**

You will need the full path to your `Vcpkg` repository for linking the `Vcpkg` toolchain file.

Open `settings.json` by selecting `File->Preferences->Settings`.
In the newly opened window, click the editor button to switch to text editing mode. See mouse cursor on the image below.

<img src="https://github.com/stwe/MDCII/blob/main/resources/doc/VsSettings.png" width="555" height="78" alt="" />

Now add the following line somewhere within the `settings.json` file

```json
"cmake.configureSettings": {
    "CMAKE_TOOLCHAIN_FILE": "/home/your_name/vcpkg/scripts/buildsystems/vcpkg.cmake"
}
```

It is quite possible that some CMake caches have to be deleted or reloaded at this point.

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
