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
* [Dear ImGui](https://github.com/ocornut/imgui)
* [Magic Enum C++](https://github.com/Neargye/magic_enum)
* [FreeType](https://freetype.org/)
* [Protocol Buffers](https://github.com/protocolbuffers/protobuf)
* C++ 17 Compiler (Gcc or MSVC)

Setting up a C++ project is always a pain. A few notes for the following explanations:

* First, [install](https://docs.conan.io/en/latest/installation.html) the Conan package manager locally.
* Always use the *latest Conan package manager*.
* The Conan profile should always specify the *correct version of the compiler*.
* On Linux, the Conan profile should *configure the package manager* to install any missing system packages.
* Especially on Windows with MinGW it is important that `gcc` and `cmake` are *in the path*.
* It might be a good idea to use a newer or other version of ImGui or Protocol Buffers. It's not (for now)!
  Additional things would have to be changed for this.

### Platforms

* [Win with Visual Studio and Premake](#win-with-visual-studio-2022-and-premake)
* [Win with MinGW64 and Cmake](#win-with-mingw64-and-cmake)
* [Linux and Cmake](#linux-with-cmake)

#### Win with Visual Studio 2022 and Premake

The Conan profile can look like this:

```txt
[settings]
os=Windows
os_build=Windows
arch=x86_64
arch_build=x86_64
compiler=Visual Studio
compiler.version=16
build_type=Release
[options]
[build_requires]
[env]
```

As this project relies on multiple 3rd-Party Libs, I created a `conanfile_win.txt` with all the requirements.

```txt
[requires]
glfw/3.3.8
glew/2.2.0
glm/0.9.9.8
spdlog/1.11.0
imgui/1.86
magic_enum/0.8.2
freetype/2.12.1
protobuf/3.21.1
gtest/1.12.1
zlib/1.2.13

[generators]
premake
```

My favor build configuration tool for Windows is Premake5. Premake5 can generate Makefiles and Visual Studio Solutions with a single description file for cross-platform projects.
Download [Premake5](https://premake.github.io/download) to your preferred location.

Complete the installation of requirements for the project running:

```bash
$ conan install conanfile_win.txt -s build_type=Debug
```

or

```bash
$ conan install conanfile_win.txt -s build_type=Release
```

followed by:

```bash
$ premake5 vs2022
```


#### Win with MinGW64 and Cmake

The Conan profile can look like this:

```txt
[settings]
os=Windows
os_build=Windows
arch=x86_64
arch_build=x86_64
compiler=gcc
compiler.version=11.2
compiler.libcxx=libstdc++11
build_type=Release
[options]
[build_requires]
[env]
```

I created a `conanfile.txt` with all the requirements.

```txt
[requires]
glfw/3.3.8
glew/2.2.0
glm/0.9.9.8
spdlog/1.11.0
imgui/1.86
magic_enum/0.8.2
freetype/2.12.1
protobuf/3.21.1
gtest/1.12.1
zlib/1.2.13

[generators]
cmake
```

Complete the installation of requirements for the project running:

```bash
$ conan install conanfile.txt -s build_type=Debug --build missing
```

or

```bash
$ conan install conanfile.txt -s build_type=Release --build missing
```

If another profile (e.g. `default_mingw`) is used, this can be specified with:

```bash
$ conan install conanfile.txt -s build_type=Debug --build missing --profile default_mingw
```


#### Linux with Cmake

The following steps work on a `.deb` based Linux (e.g. Debian or Mint). Arch based systems have not been tested yet.

Install some needed packages:

```bash
sudo apt install git cmake gdb build-essential python3-pip libgl-dev libglu1-mesa-dev libgl1-mesa-dev pkg-config
```

Install Conan with:

```bash
sudo pip3 install conan
```

Create the Conan profile with:

```bash
$ conan profile new default --detect  # Generates default profile detecting GCC and sets old ABI
```

Conan will set the `compiler.libcxx` to the old ABI for backwards compatibility. You must change this with the following command:

```bash
$ conan profile update settings.compiler.libcxx=libstdc++11 default  # Sets libcxx to C++11 ABI
```

On my Debian testing system (bookworm) the Conan profile currently looks like this:

**Note: The [conf] section should look like this so that missing packages are automatically installed. Conan will then ask for the password.**

```txt
[settings]
os=Linux
os_build=Linux
arch=x86_64
arch_build=x86_64
compiler=gcc
compiler.version=12.2
compiler.libcxx=libstdc++11
build_type=Release

[options]

[build_requires]

[env]

[conf]
tools.system.package_manager:tool=apt-get
tools.system.package_manager:mode=install
tools.system.package_manager:sudo=True
```

I created a `conanfile.txt` with all the requirements.

```txt
[requires]
glfw/3.3.8
glew/2.2.0
glm/0.9.9.8
spdlog/1.11.0
imgui/1.86
magic_enum/0.8.2
freetype/2.12.1
protobuf/3.21.1
gtest/1.12.1
zlib/1.2.13

[generators]
cmake
```

Complete the installation of requirements for the project running:

```bash
$ conan install conanfile.txt -s build_type=Debug --build missing
```

or

```bash
$ conan install conanfile.txt -s build_type=Release --build missing
```

Build with:

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ cd src/bin
$ ./MDCII
```

## Running MDCII

The original game is needed.

For a program start so called tile atlas graphics are needed, which have to be created first. For this purpose a
`TileAtlasCreator` is located in the `install` directory. See [README](https://github.com/stwe/MDCII/blob/main/install/TileAtlasCreator/README.md).

In the MDCII `config.ini` the paths must be adapted.

**The first start takes longer, because the `haeuser.cod` will be parsed.**

## Devlog

**28.01.2023** Add a tag for v0.1.

**25.01.2023** The project includes a world generator that can create an island based on Perlin Noise.
The class is in a very early stage of development, so we shouldn't expect too much right now. An example of an
island created with `Seed = 4181` and a `Frequency of 0.086f` can be seen here:

<img src="https://github.com/stwe/MDCII/blob/main/resources/devlog/25-01-2023.png" width="500" height="400" alt="" />

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
