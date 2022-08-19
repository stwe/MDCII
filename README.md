# MDCII

Ein weiteres 1602 Projekt. Der Fokus liegt hier jedoch nicht in dem 
Einlesen der Originaldateien, sondern darin, eine editierbare isometrische
Spielewelt zu erschaffen.

Es wird das Originalspiel benötigt. In der `config.ini` müssen die Pfade angepasst werden.

**Der erste Start dauert länger, da die `haeuser.cod` eingelesen wird.**

**Update vom 17.08.2022**
Derzeit können Handwerksbetriebe in beliebiger Rotation auf der Karte platziert und auch wieder zerstört werden.
Es ist möglich, die Karte zu drehen. Darüber hinaus kann die Map gespeichert und wieder geladen werden. Um das
Savegame beim Start anzuzeigen, muss nur der Eintrag `start_map` in der `config.ini` geändert werden. 
Wie im Originalspiel sind drei Zoomstufen möglich.


[![Codacy Badge](https://app.codacy.com/project/badge/Grade/50ffb066523c4064ab513bef2f8d4628)](https://www.codacy.com/gh/stwe/MDCII/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=stwe/MDCII&amp;utm_campaign=Badge_Grade)


<img src="https://github.com/stwe/MDCII/blob/main/resources/devlog/25-06-2022.png" width="500" height="400" />


## Build

### Requirements

* [GLFW](https://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [GLM](https://github.com/g-truc/glm)
* [spdlog](https://github.com/gabime/spdlog)
* [Dear ImGui](https://github.com/ocornut/imgui)
* [Magic Enum C++](https://github.com/Neargye/magic_enum)
* [FreeType](https://freetype.org/)
* [Protocol Buffers](https://github.com/protocolbuffers/protobuf)
* C++ 17 Compiler

### Platforms

* Win with Visual Studio and Premake
* Win with MinGW64 (Gcc/Cmake)
* Linux

### Build instructions

Setting up a C++ project is always a pain. A few notes for the following explanations:

- First, [install](https://docs.conan.io/en/latest/installation.html) the Conan package manager locally.
- Always use the latest Conan package manager.
- The Conan profile should always specify the correct version of the compiler.
- Especially on Windows with MinGW it is important that `gcc` and `cmake` are in the path.
- It might be a good idea to use a newer or other version of ImGui or Protocol Buffers. It's not (for now)!
Additional things would have to be changed for this.

#### Win with Visual Studio and Premake

As this project relies on multiple 3rd-Party Libs, I created a `conanfile.txt` with all the requirements.

```txt
[requires]
glfw/3.3.7
glew/2.2.0
glm/0.9.9.8
spdlog/1.10.0
imgui/1.86
magic_enum/0.8.0
freetype/2.12.1
protobuf/3.21.1

[generators]
premake
```

The Conan profile should look like this:

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
$ premake5 vs2019
```


#### Win with MinGW64 (Gcc/Cmake)

I created a `conanfile.txt` with all the requirements.

```txt
[requires]
glfw/3.3.7
glew/2.2.0
glm/0.9.9.8
spdlog/1.10.0
imgui/1.86
magic_enum/0.8.0
freetype/2.12.1
protobuf/3.21.1

[generators]
cmake
```

The Conan profile should look like this:

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


#### Linux

If you are using GCC compiler >= 5.1, Conan will set the `compiler.libcxx` to the old ABI for backwards compatibility. You can change this with the following commands:

```bash
$ conan profile new default --detect  # Generates default profile detecting GCC and sets old ABI
$ conan profile update settings.compiler.libcxx=libstdc++11 default  # Sets libcxx to C++11 ABI
```

I created a `conanfile.txt` with all the requirements.

```txt
[requires]
glfw/3.3.7
glew/2.2.0
glm/0.9.9.8
spdlog/1.10.0
imgui/1.86
magic_enum/0.8.0
freetype/2.12.1
protobuf/3.21.1

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

**If the following error occurs: `ERROR: opengl/system: Error in package_info() method, line 80`, then `pkg-config` must be installed first.**

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ cd src/bin
$ ./MDCII
```

## License

MDCII is licensed under the GPL-2.0 License, see [LICENSE](https://github.com/stwe/MDCII/blob/main/LICENSE) for more information.
