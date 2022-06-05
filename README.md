# MDCII

Ein weiteres 1602 Projekt. Der Fokus liegt hier jedoch nicht in dem 
Einlesen der Originaldateien, sondern darin, eine editierbare isometrische
Spielewelt zu erschaffen.


[![Codacy Badge](https://app.codacy.com/project/badge/Grade/50ffb066523c4064ab513bef2f8d4628)](https://www.codacy.com/gh/stwe/MDCII/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=stwe/MDCII&amp;utm_campaign=Badge_Grade)


<img src="https://github.com/stwe/MDCII/blob/main/resources/devlog/first.png" width="500" height="400" />


## Build

### Requirements

* [GLFW](https://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [GLM](https://github.com/g-truc/glm)
* [spdlog](https://github.com/gabime/spdlog)
* [Dear ImGui](https://github.com/ocornut/imgui)
* [NLohmann Json](https://github.com/nlohmann/json)
* [Magic Enum C++](https://github.com/Neargye/magic_enum)
* [FreeType](https://freetype.org/)
* [Protocol Buffers](https://github.com/protocolbuffers/protobuf)
* C++ 17 Compiler

### Platforms

* Win
* Linux

### Build instructions

#### Win

First, [install](https://docs.conan.io/en/latest/installation.html) the Conan package manager locally.

As this project relies on multiple 3rd-Party Libs, I created a `conanfile.txt` with all the requirements.

```txt
[requires]
glfw/3.3.7
glew/2.2.0
glm/0.9.9.8
spdlog/1.10.0
imgui/1.86
nlohmann_json/3.10.5
magic_enum/0.8.0
freetype/2.12.1
protobuf/3.21.1

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
$ premake5 vs2019
```


#### Linux

First, [install](https://docs.conan.io/en/latest/installation.html) the Conan package manager locally.

If you are using GCC compiler >= 5.1, Conan will set the `compiler.libcxx` to the old ABI for backwards compatibility. You can change this with the following commands:

```bash
$ conan profile new default --detect  # Generates default profile detecting GCC and sets old ABI
$ conan profile update settings.compiler.libcxx=libstdc++11 default  # Sets libcxx to C++11 ABI
```

As this project relies on multiple 3rd-Party Libs, I created a `conanfile.txt` with all the requirements.

```txt
[requires]
glfw/3.3.7
glew/2.2.0
glm/0.9.9.8
spdlog/1.10.0
imgui/1.86
nlohmann_json/3.10.5
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
