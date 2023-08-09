workspace "GameEngine"
    architecture "x64"
    startproject "sandBox"
    configurations {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "GameEngine/vendor/GLFW/include"

include "GameEngine/vendor/GLFW"

project "GameEngine"
    location "GameEngine"
    kind "SharedLib"
    language "C++"
    runtime "Debug"


    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "hzpch.h"
    pchsource "GameEngine/src/hzpch.cpp"

    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}"
    }

    links{
        "GLFW",
        "opengl32.lib"
    }

    postbuildcommands{
        ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"

        defines{

            "EG_PLATFORM_WINDOWS",
            "EG_BUILD_DLL"
        }
    filter "configurations:Debug"
        defines "EG_DEBUG"
        --buildoptions "/MDd"
        symbols "On"
    filter "configurations:Release"
        defines "EG_RELEASE"
        optimize "On"
    filter "configurations:Dist"
        defines "EG_DIST"
        optimize "On"

project "sandBox"
    location "sandBox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "GameEngine/vendor/spdlog/include",
        "GameEngine/src"
    }

    links {
        "GameEngine"
    }
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines{

            "EG_PLATFORM_WINDOWS",
        }
    filter "configurations:Debug"
        defines "EG_DEBUG"
        symbols "On"
    filter "configurations:Release"
        defines "EG_RELEASE"
        optimize "On"
    filter "configurations:Dist"
        defines "EG_DIST"
        optimize "On"