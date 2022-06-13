include("conanbuildinfo.premake.lua")

workspace "MDCII"
    conan_basic_setup()

    architecture "x64"
    startproject "MDCII"

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

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    linkoptions
    {
        conan_exelinkflags,
        "/IGNORE:4099"
    }

    files
    {
        "/Dev/MDCII/src/**.h",
        "/Dev/MDCII/src/**.hpp",
        "/Dev/MDCII/src/**.cpp",
        "/Dev/MDCII/src/**.cc",
        "/Dev/MDCII/src/**.proto"
    }

    includedirs
    {
        "/Dev/MDCII/src"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "MDCII_DEBUG_BUILD", "GLFW_INCLUDE_NONE", "_CRT_SECURE_NO_WARNINGS" }
        runtime "Debug"
        symbols "On"

    -- disable Basic Runtime Checks
    -- debug with inlines (use /Ob1)
    -- disable Edit And Continue (use /Zi)
    -- disable Just My Code debugging
    filter "configurations:FastDebug"
        defines { "MDCII_DEBUG_BUILD", "GLFW_INCLUDE_NONE", "_CRT_SECURE_NO_WARNINGS" }
        runtime "Debug"
        symbols "On"
        flags { "NoRuntimeChecks" }
        buildoptions "/Ob1"
        editandcontinue "Off"
        justmycode "Off"

    filter "configurations:Release"
        defines { "GLFW_INCLUDE_NONE", "_CRT_SECURE_NO_WARNINGS" }
        runtime "Release"
        optimize "On"
