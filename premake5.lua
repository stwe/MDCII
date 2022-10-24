include("conanbuildinfo.premake.lua")

workspace "MDCII"
    conan_basic_setup()

    architecture "x64"

    configurations
    {
        "Debug",
        "FastDebug",
        "Release"
    }

    floatingpoint "Fast"
    flags "MultiProcessorCompile"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "MDCII"
    location "/Dev/MDCII"
    architecture "x64"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    characterset "Unicode"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    linkoptions
    {
        conan_exelinkflags,
        "/IGNORE:4099"
    }

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp",
        "src/**.cc",
        "src/**.proto"
    }

    includedirs
    {
        "src"
    }

    postbuildcommands
    {
        "{COPY} config.ini %{cfg.targetdir}",
        "{COPY} resources/ %{cfg.targetdir}/resources"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "MDCII_DEBUG_BUILD", "GLFW_INCLUDE_NONE", "_CRT_SECURE_NO_WARNINGS", "GLM_ENABLE_EXPERIMENTAL" }
        runtime "Debug"
        symbols "On"

    -- disable Basic Runtime Checks
    -- debug with inlines (use /Ob1)
    -- disable Edit And Continue (use /Zi)
    -- disable Just My Code debugging
    filter "configurations:FastDebug"
        defines { "MDCII_DEBUG_BUILD", "GLFW_INCLUDE_NONE", "_CRT_SECURE_NO_WARNINGS", "GLM_ENABLE_EXPERIMENTAL" }
        runtime "Debug"
        symbols "On"
        flags { "NoRuntimeChecks" }
        buildoptions "/Ob1"
        editandcontinue "Off"
        justmycode "Off"

    filter "configurations:Release"
        defines { "GLFW_INCLUDE_NONE", "_CRT_SECURE_NO_WARNINGS", "GLM_ENABLE_EXPERIMENTAL" }
        runtime "Release"
        optimize "On"

project "MDCII_TEST"
    location "/Dev/MDCII"
    architecture "x64"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    characterset "Unicode"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    linkoptions
    {
        conan_exelinkflags,
        "/IGNORE:4099"
    }

    links
    {
        "gtest_main"
    }

    files
    {
        "tests/**.cpp"
    }

    includedirs
    {
        "src"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "_CRT_SECURE_NO_WARNINGS" }
        runtime "Debug"
        symbols "On"
